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

#include "CameraCapture.hpp"
#include "Timer.hpp"

#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <iostream>
using namespace std;

int CameraCapture::initialize()
{
	if (initialized)
		return 0;

	video.open(id);

	if (!video.isOpened())
	{
		cout << "ERROR: CameraCapture for video " << id << " could not initialize." << endl;

		cout << "For now, using stock video..." << endl;
		switch (id)
		{
		case 0:
			video = VideoCapture("../../video1.mpeg");
			break;
		case 1:
			video = VideoCapture("../../video2.mpeg");
			break;
		default:
			cout << "No stock video exists" << endl;
			return -1;
		}

		if (!video.isOpened())
			return -1;
	}
	else
	{
		setSize(width, height);
	}

	initialized = true;
	return 0;
}

void CameraCapture::setSize(int w, int h)
{
	width = w;
	height = h;

	if (video.isOpened())
	{
		video.set(CV_CAP_PROP_FRAME_WIDTH, width);
		video.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	}
}

int CameraCapture::start()
{
	// If you forgot to initialize, I'll do it for you
	if (!initialized)
	{
		if (initialize())
			return -1;
	}
	
	doneEvent = CreateEvent( 
        NULL,               // default security attributes
        true,				// manual-reset?
        false,              // initial state 
        NULL				// object name
        );

    if (doneEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return -1;
    }

	threadHandle = CreateThread(
			NULL,				// default security attributes
			0,					// use default stack size  
			StartThread,		// thread function name
			this,				// argument to thread function 
			0,					// use default creation flags 
			NULL);				// returns the thread identifier

	if (threadHandle == NULL)
	{
		cout << "Could not start CameraCapture " << id << " thread." << endl;
		return -1;
	}
	
	running = true;
	return 0;
}

int CameraCapture::stop()
{
	if (!running)
		return 0;

	running = false;
	DWORD returnCode;
	
	// This forces all other CamCap threads to close too
	SetEvent(startEvent);
	SetEvent(stopEvent);

	do
	{
		Sleep(10);
		GetExitCodeThread(threadHandle, &returnCode);
	}
	while (returnCode == STILL_ACTIVE);

	return 0;
}

int CameraCapture::run()
{
	cout << "Started CameraCapture " << id << " thread." << endl;

	DWORD waitResult;
	do
	{
		// Wait for startEvent
		waitResult = WaitForSingleObject( 
			startEvent,		// event handle
			INFINITE);		// indefinite wait

		switch (waitResult) 
		{
			// Event object was signaled
			case WAIT_OBJECT_0:
				break; 

			// An error occurred
			default: 
				printf("Error in CaptureThread while waiting for startEvent: (%d)\n", GetLastError()); 
				return -1;
		}

		getFrame();

		// Tell the VideoStitcher we're done.
		SetEvent(doneEvent);

		// Wait for stopEvent
		waitResult = WaitForSingleObject( 
			stopEvent,			// event handle
			INFINITE);			// indefinite wait

		switch (waitResult) 
		{
			// Event object was signaled
			case WAIT_OBJECT_0:
				break; 

			// An error occurred
			default: 
				printf("Error in CaptureThread while waiting for stopEvent: (%d)\n", GetLastError()); 
				return -1;
		}

	} while (running);

	cout << "Ending CameraCapture thread " << id << '.' << endl;
	running = false;
	return 0;
}

void CameraCapture::getFrame()
{
	Timer::send(Timer::Camera, id, Timer::CamTimeval::Start);

	if (video.isOpened() && video.grab())
	{
		if (video.retrieve(frame))
		{
			if (frame.rows > 0 && frame.cols > 0 && inverted)
			{
				try
				{
					Point2f src_center(frame.cols/2.0F, frame.rows/2.0F);
					Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
					Mat rotated;
					warpAffine(frame, rotated, rot_mat, frame.size());
					frame = rotated;
				}
				catch (Exception)
				{
					frame = Mat(0,0,0);
				}
			}
		}
	}

	Timer::send(Timer::Camera, id, Timer::CamTimeval::End);
}
