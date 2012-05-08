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

#ifndef CAPTURETHREAD_HPP
#define CAPTURETHREAD_HPP

#include "Timer.hpp"
#include "Config.hpp"

#include <Windows.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;

class __declspec(dllexport) CameraCapture
{
public:

	int id;
	HANDLE doneEvent;
	Mat frame;

	/// Constructor
	CameraCapture(int id,
			int width,
			int height,
			bool inverted,
			HANDLE startEvent,
			HANDLE stopEvent)
		:id(id),
			width(width),
			height(height),
			inverted(inverted),
			startEvent(startEvent),
			stopEvent(stopEvent)
	{
		running = false;
		initialized = false;
		doneEvent = INVALID_HANDLE_VALUE;
		threadHandle = INVALID_HANDLE_VALUE;
	}

	~CameraCapture()
	{
		if (running)
			stop();

		if (doneEvent != INVALID_HANDLE_VALUE)
		{
			CloseHandle(doneEvent);
			doneEvent = INVALID_HANDLE_VALUE;
		}
	}

	int initialize();
	void setSize(int w, int h);
	int start();
	int stop();

	void getFrame();

private:

	bool inverted, running, initialized;
	int width, height;
	HANDLE threadHandle;
	HANDLE startEvent;
	HANDLE stopEvent;

	VideoCapture video;

	// Thread entry point
	static DWORD WINAPI StartThread(LPVOID arg)
	{
		return ((CameraCapture*)arg)->run();
	}

	// Thread function
	int run();
};

#endif
