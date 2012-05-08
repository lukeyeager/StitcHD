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

#include "VideoStitcher.hpp"
#include "ImageStitcher.hpp"
#include "Utils.h"

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

VideoStitcher::VideoStitcher(Config& c)
	:config(c),
	timer(c),
	imageStitcher()
{
	running = false;
	recording = false;
	hmgCntlRunning = false;
	hmgCntlThreadHandle = INVALID_HANDLE_VALUE;
	hmgLatency = -1;
}

VideoStitcher::~VideoStitcher()
{
	// Run this in case it hasn't been already
	stop();

	for (int i=0; i<homographiers.size(); i++)
		delete homographiers[i];

	for (int i=0; i<cameraCaptures.size(); i++)
		delete cameraCaptures[i];
}

int VideoStitcher::ViewCameras(Config& settings)
{
	Timer timer(settings);

	if (timer.start())
		return -1;

	vector<CameraCapture> devices;

	for (int i=0; i<settings.camCount; i++)
	{
		devices.push_back(
			CameraCapture(i,
				settings.camSizes[i][0], 
				settings.camSizes[i][1],
				settings.camInverted[i],
				NULL, 
				NULL));

		if (devices.back().initialize())
			return -1;
	}

	vector<string> windowNames;
	for (int i=0; i<devices.size(); i++)
	{
		stringstream name;
		name << "Frame " << i;
		windowNames.push_back(name.str());

		namedWindow(name.str());
	}

	do
	{
		for (int i=0; i<devices.size(); i++)
		{
			devices[i].getFrame();

			if (devices[i].frame.cols > 0 && devices[i].frame.rows > 0)
			{
				imshow(windowNames[i], devices[i].frame);
			}
			else
			{
				cout << "Bad frame" << endl;
			}
		}
	} while (waitKey(10) < 0);

	destroyAllWindows();
	timer.stop();

	timer.writeToFile();

	cout << endl;
	return 0;
}

int VideoStitcher::start()
{
	if (timer.start())
		return -1;

	if (startCameraCaptures())
		return -1;

	if (config.hmgCount > 0)
		if (startHmgController())
			return -1;

	running = true;
	return 0;
}

int VideoStitcher::startCameraCaptures()
{
	// Create startCaptureEvent
	startCapEvent = CreateEvent( 
        NULL,               // default security attributes
        true,				// manual-reset?
        false,              // initial state 
        NULL				// object name
        );

    if (startCapEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return -1;
    }

	// Create stopCaptureEvent
	stopCapEvent = CreateEvent( 
        NULL,               // default security attributes
        true,				// manual-reset?
        true,				// initial state 
        NULL				// object name
        );

    if (stopCapEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return -1;
    }

	// Create mutex
	framesMutex = CreateMutex( 
        NULL,			// default security attributes
        false,			// initial state
        NULL);			// name

    if (framesMutex == NULL) 
    {
        printf("CreateMutex error: %d\n", GetLastError());
        return -1;
    }

	// Create threads
	for (int i=0; i<config.camCount; i++)
	{
		cameraCaptures.push_back(
			new CameraCapture(
				i,
				config.camSizes[i][0],
				config.camSizes[i][1],
				config.camInverted[i],
				startCapEvent,
				stopCapEvent ));

		if (cameraCaptures.back()->start())
			return -1;
	}

	return 0;
}

int VideoStitcher::startHmgController()
{
	startHmgEvent = CreateEvent( 
        NULL,               // default security attributes
        true,				// manual-reset?
        false,              // initial state 
        NULL				// object name
        );

    if (startHmgEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return -1;
    }

	stopHmgEvent = CreateEvent( 
        NULL,               // default security attributes
        true,				// manual-reset?
        true,				// initial state 
        NULL				// object name
        );

    if (stopHmgEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return -1;
    }

	for (int i=0; i<config.hmgCount; i++)
	{
		homographiers.push_back(
			new Homographier(i,
				config,
				startHmgEvent,
				stopHmgEvent,
				config.hmgDirections[i][0],
				config.hmgDirections[i][1])
			);

		if (homographiers.back()->start())
			return -1;
	}

	hmgCntlRunning = true;
	
	hmgCntlThreadHandle = CreateThread(
			NULL,				// default security attributes
			0,					// use default stack size  
			StartHmgController,	// thread function name
			this,				// argument to thread function 
			0,					// use default creation flags 
			NULL);				// returns the thread identifier

	if (hmgCntlThreadHandle == NULL)
	{
		cout << "Could not start HmgController thread." << endl;
		hmgCntlRunning = false;
		return -1;
	}

	return 0;
}

int VideoStitcher::getImage()
{
	// Wait for framesMutex
	DWORD waitResult = WaitForSingleObject( 
		framesMutex,	// event handle
		5000);			// timeout

	switch (waitResult) 
	{
        // This thread got ownership of the mutex
        case WAIT_OBJECT_0:
            break;

        case WAIT_TIMEOUT:
			ReleaseMutex(framesMutex);
            printf("Mutex wait timed out.\n");
            return -1;

		default:
			ReleaseMutex(framesMutex);
			printf("Mutex wait error: %d\n", GetLastError()); 
            return -2;
	}

	ResetEvent(stopCapEvent);
	SetEvent(startCapEvent);

	HANDLE done[MAX_CAMERAS];
	for (int i=0; i<cameraCaptures.size(); i++)
		done[i] = cameraCaptures[i]->doneEvent;

	waitResult = WaitForMultipleObjects(
        config.camCount,		// number of handles in array
        done,					// array of thread handles
        true,					// wait until all are signaled
        5000);					// timeout

    switch (waitResult) 
    {
        // All thread objects were signaled
        case WAIT_OBJECT_0: 
            break;

		case WAIT_TIMEOUT:
            printf("CameraCapture wait timed out.\n");
			ReleaseMutex(framesMutex);
            return -1;

        // An error occurred
        default: 
            printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
			ReleaseMutex(framesMutex);
            return -1;
    }

	ResetEvent(startCapEvent);

	for (int i=0; i<cameraCaptures.size(); i++)
		ResetEvent(done[i]);
	
	SetEvent(stopCapEvent);

	// Get frames
	
	Mat frames[MAX_CAMERAS];
	for (int i=0; i<cameraCaptures.size(); i++)
		cameraCaptures[i]->frame.copyTo(frames[i]);
	
	ReleaseMutex(framesMutex);

	for (int i=0; i<cameraCaptures.size(); i++)
	{
		if (frames[i].cols <= 0 || frames[i].rows <= 0)
			return -1;
	}
	
	Mat hmgs[MAX_CAMERAS];

	for (int i=0; i<homographiers.size(); i++)
	{
		homographiers[i]->homography.copyTo(hmgs[i]);
		
		if (config.showMatches && homographiers[i]->matchesFrame.rows > 0 && homographiers[i]->matchesFrame.cols > 0)
		{
			stringstream name;
			name << "Homographier " << i;
			imshow(name.str(), homographiers[i]->matchesFrame);
		}
	}


	Timer::send(Timer::Stitch, 0, Timer::StitchTimeval::Start);
	
#if COMPILE_GPU == 1
	displayFrame = imageStitcher.stitchImages_GPU(frames, hmgs, config);
#else
	displayFrame = imageStitcher.stitchImages(frames, hmgs, config);
#endif

	Timer::send(Timer::Stitch, 0, Timer::StitchTimeval::End);

	if (displayFrame.cols <= 0 || displayFrame.rows <= 0)
		return -1;

	if (recording)
		recorder->write(displayFrame);

	return 0;
}

int VideoStitcher::stop()
{
	if (!running)
		return 0;

	if (recording)
		stopRecording();

	// Stop HmgController
	stopHmgController();

	closeMatchFrames();
	
	// Stop CameraCaptures
	SetEvent(startCapEvent);
	SetEvent(stopCapEvent);
	for (int i=0; i<cameraCaptures.size(); i++)
		cameraCaptures[i]->stop();

	// Stop Timer
	timer.stop();

	running = false;
	
	// Close event handles
	CloseHandle(startCapEvent);
	CloseHandle(stopCapEvent);

	cout << "Frame resolution: " << displayFrame.cols << "x" << displayFrame.rows << endl;

	return 0;
}

int VideoStitcher::closeMatchFrames()
{
	for (int i=0; i<config.hmgCount; i++)
	{
		stringstream name;
		name << "Homographier " << i;
		try
		{
			destroyWindow(name.str());
		}
		catch (Exception)
		{
		}
	}
	return 0;
}

int VideoStitcher::runHmgController()
{
	cout << "Started HmgController." << endl;
	
	vector<clock_t> latencies(5);
	for (int i=0; i<latencies.size(); i++)
		latencies[i] = -1;
	int latencyIndex = 0;

	do
	{
		// Stop homographiers
		ResetEvent(startHmgEvent);
		SetEvent(stopHmgEvent);

		// Wait for framesMutex
		DWORD waitResult = WaitForSingleObject( 
			framesMutex,	// event handle
			5000);			// timeout

		switch (waitResult) 
		{
			// This thread got ownership of the mutex
			case WAIT_OBJECT_0:
				break;

			case WAIT_TIMEOUT:
				ReleaseMutex(framesMutex);
				printf("framesMutex wait timed out.\n");
				return -1;

			default:
				ReleaseMutex(framesMutex);
				printf("framesMutex wait error: %d\n", GetLastError()); 
				return -2;
		}

		// Copy frames
		for (int i=0; i<homographiers.size(); i++)
		{
			cameraCaptures[config.hmgTargets[i][0]]->frame.copyTo(homographiers[i]->frameA);
			cameraCaptures[config.hmgTargets[i][1]]->frame.copyTo(homographiers[i]->frameB);
		}
	
		ReleaseMutex(framesMutex);

		// Copy configuration
		Config c = this->config;

		for (int i=0; i<homographiers.size(); i++)
			homographiers[i]->config = c;

		// Start the homographiers
		ResetEvent(stopHmgEvent);
		SetEvent(startHmgEvent);
		
		// Update the latency
		latencies[latencyIndex] = clock();
		hmgLatency = Timer::msTime(latencies[(latencyIndex + 1) % latencies.size()], latencies[latencyIndex]) / latencies.size();
		latencyIndex = (latencyIndex + 1) % latencies.size();

		HANDLE done[MAX_CAMERAS];
		for (int i=0; i<homographiers.size(); i++)
			done[i] = homographiers[i]->doneEvent;

		waitResult = WaitForMultipleObjects(
			homographiers.size(),	// number of handles in array
			done,					// array of thread handles
			true,					// wait until all are signaled
			15000);					// timeout

		switch (waitResult) 
		{
			// All thread objects were signaled
			case WAIT_OBJECT_0: 
				break;

			case WAIT_TIMEOUT:
				printf("HmgController wait timed out.\n");
				ReleaseMutex(framesMutex);
				return -1;

			// An error occurred
			default: 
				printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
				ReleaseMutex(framesMutex);
				return -1;
		}

		for (int i=0; i<homographiers.size(); i++)
			ResetEvent(done[i]);

	} while(hmgCntlRunning);

	cout << "Ending HmgController." << endl;
	hmgCntlRunning = false;
	return 0;
}

int VideoStitcher::stopHmgController()
{
	if (!hmgCntlRunning)
		return 0;

	hmgCntlRunning = false;
	SetEvent(startHmgEvent);
	SetEvent(stopHmgEvent);

	for (int i=0; i<homographiers.size(); i++)
	{
		// Just continue if one fails to stop
		homographiers[i]->stop();
	}

	DWORD returnCode;

	do
	{
		Sleep(10);
		GetExitCodeThread(hmgCntlThreadHandle, &returnCode);
	}
	while (returnCode == STILL_ACTIVE);

	CloseHandle(startHmgEvent);
	CloseHandle(stopHmgEvent);

	return 0;
}

int VideoStitcher::startRecording()
{
	if (recording)
		return -1;

	recorder = new VideoWriter(videoOutputFileName(), CV_FOURCC('P','I','M','1'), 20, Size(displayFrame.cols, displayFrame.rows));

	recording = true;

	return 0;
}

int VideoStitcher::stopRecording()
{
	if (!recording)
		return -1;
	
	recording = false;

	recorder->~VideoWriter();
	recorder = NULL;

	return true;
}