/*
This file is part of StitcHD.

StitcHD is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

StitcHD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with StitcHD.  If not, see <http://www.gnu.org/licenses/>.
*/

#define WIN32_LEAN_AND_MEAN 1
#include <WinSock2.h>

#include "Timer.hpp"
#include "Utils.h"

#include <sstream>
#include <fstream>
using namespace std;

// Setting static const values
const int Timer::Port = 1200;	//Whoop
const int Timer::BufferSize = 63;

Timer::Timer(const Config c)
	:config(c),
	camTimevals(c.camCount),
	hmgTimevals(c.hmgCount)
{
	running = false;
}

int Timer::start()
{
	if (running)
		return -1;

	// Initialize Winsock
	WSADATA wsaData;
	int rc = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (rc != 0)
	{
        printf("WSAStartup failed with error: %d\n", rc);
        return 1;
    }

	running = true;

	threadHandle = CreateThread(
		NULL,				// default security attributes
		0,					// use default stack size  
		StartThread,		// thread function name
		this,				// argument to thread function 
		0,					// use default creation flags 
		NULL);				// returns the thread identifier

	if (threadHandle == NULL) 
	{
		printf("CreateThread failed (%d)\n", GetLastError());
		running = false;
		return -1;
	}

	cout << "Waiting for Timer to initialize..." << endl;
	Sleep(100);
	send(Stitch, 0, StitchTimeval::Start);
	Sleep(100);
	if (stitchTimevals.size() == 0 )
	{
		cout << "ERROR: Timer did not start." << endl;
		stop();
		return -1;
	}
	else
		return 0;
}

int Timer::stop()
{
	if (!running)
		return 0;

	running = false;
	DWORD returnCode;

	do
	{
		if (send(Stitch, -1, -1))
			return -1;

		Sleep(10);
		GetExitCodeThread(threadHandle, &returnCode);

	} while (returnCode == STILL_ACTIVE);

	return 0;
}

// See http://msdn.microsoft.com/en-us/library/ms740148
int Timer::send(Type type, int id, int subType)
{
	clock_t time = clock();

	char message[BufferSize];

	stringstream ss;
	ss << type << ' ' << id << ' ' << subType << ' ' << time;

	strcpy(message, ss.str().c_str());
	
	// Set up the RecvAddr structure with the IP address of
    // the receiver and the specified port number.
	sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(Port);
	recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// Create a socket for sending data
    SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        return -1;
    }

    // Send a datagram to the receiver
	int numBytes = sendto(
		sendSocket,
		message,
		BufferSize,
		0,
		(SOCKADDR *) & recvAddr,
		sizeof(recvAddr) );

	if (numBytes == SOCKET_ERROR)
	{
        wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
        return -1;
    }

	if (0 != closesocket(sendSocket))
	{
		wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
        return -1;
	}

	return 0;
}

// See http://msdn.microsoft.com/en-us/library/ms740120
int Timer::run()
{
	cout << "Started Timer." << endl;

	// Create a receiver socket
	SOCKET recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (recvSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error %d\n", WSAGetLastError());
        return 1;
    }

	// Bind the socket to any incoming address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(Port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
    if (0 != bind(recvSocket, (SOCKADDR *) & serverAddr, sizeof(serverAddr)))
	{
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        return 1;
    }

	while (running)
	{
		char recvBuf[BufferSize];
		sockaddr_in senderAddr;
		int senderAddrSize = sizeof(senderAddr);

		int numBytes = recvfrom(
			recvSocket,
			recvBuf,
			BufferSize,
			0,
			(SOCKADDR *) & senderAddr,
			&senderAddrSize);

		if (!running)	// Don't try to process the data if we're through
			continue;

		if (numBytes == SOCKET_ERROR)
		{
			wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
		}
		else
		{
			stringstream ss(recvBuf);
			int _type;
			ss >> _type;
			Type type = Type(_type);

			int id;
			ss >> id;
			int subType;
			ss >> subType;

			int time;
			ss >> time;

			//cout << "Received: " << type << ' ' << id << ' ' << subType << ' ' << time << endl;

			switch (type)
			{
			case Camera:
				if (id >= 0 && id < camTimevals.size())
				{
					if (subType == CamTimeval::Start)
						camTimevals[id].push_back(CamTimeval());
					
					camTimevals[id].back().times[subType] = time;
				}
				break;

			case Homography:
				if (id >= 0 && id < hmgTimevals.size())
				{
					if (subType == HmgTimeval::Start)
						hmgTimevals[id].push_back(HmgTimeval());

					
					hmgTimevals[id].back().times[subType] = time;
				}
				break;

			case Stitch:
				if (subType == StitchTimeval::Start)
					stitchTimevals.push_back(StitchTimeval());
					
				stitchTimevals.back().times[subType] = time;

			default:
				break;
			}
		}
    }
	
	if (SOCKET_ERROR == closesocket(recvSocket))
	{
		wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
	}

	cout << "Ending Timer thread." << endl;
	return 0;
}

int Timer::msTime(const clock_t start, const clock_t end)
{
	if (start == 0 || end == 0 || start > end)
		return -1;	// invalid

	return 1000 * (end - start) / CLOCKS_PER_SEC;
}

void Timer::writeToFile()
{
	string fileName = timerOutputFileName();

	ofstream outFile;
	outFile.open(fileName.c_str());

	if (!outFile.is_open())
	{
		cout << "ERROR: Could not open file " << fileName << endl;
	}
	else
	{
		outFile << "--- StitcHD Timing Information ---" << endl << endl;

		config.print(outFile);
		outFile << endl;

		print(outFile);

		outFile.close();
		cout << "Wrote Timer data to " << fileName << endl;
	}
}

void Timer::print(ostream& os)
{
	os << endl << "- Cameras -" << endl
		<< "ID\tStart\tTime" << endl;
	for (int i=0; i<camTimevals.size(); i++)
	{
		int count = 0;
		int sum = 0;

		for (int j=0; j<camTimevals[i].size(); j++)
		{
			int elapsed = msTime(camTimevals[i][j].times[0], camTimevals[i][j].times[1]);
			if (j > 1 && elapsed > 0)
			{
				count += 1;
				sum += elapsed;
			}
			os << i + 1
				<< '\t' << camTimevals[i][j].times[0]
				<< '\t' << elapsed
				<< endl;
		}

		if (count > 0)
			os << "Avg: " << sum / count << endl;
	}

	os << endl << "- Homographiers -" << endl
		<< "ID\tStart\tDetect\tMatch\tHmg" << endl;
	for (int i=0; i<hmgTimevals.size(); i++)
	{
		for (int j=0; j<hmgTimevals[i].size(); j++)
		{
			os << i + 1
				<< '\t' << hmgTimevals[i][j].times[0]
				<< '\t' << msTime(hmgTimevals[i][j].times[0], hmgTimevals[i][j].times[1])
				<< '\t' << msTime(hmgTimevals[i][j].times[1], hmgTimevals[i][j].times[2])
				<< '\t' << msTime(hmgTimevals[i][j].times[2], hmgTimevals[i][j].times[3])
				<< endl;
		}
	}

	os << endl << "- Stitcher -" << endl
		<< "Start\tTime" << endl;
	
	int count = 0;
	int sum = 0;
	for (int i=0; i<stitchTimevals.size(); i++)
	{
		int elapsed = msTime(stitchTimevals[i].times[0], stitchTimevals[i].times[1]);
		if (i > 1 && elapsed > 0)
		{
			count += 1;
			sum += elapsed;
		}

		os << stitchTimevals[i].times[0]
			<< '\t' << elapsed
			<< endl;
	}
	if (count > 0)
		os << "Avg: " << sum / count << endl;

	os << endl;
}