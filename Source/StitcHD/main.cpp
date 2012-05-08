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

#define MAIN_WINDOW "Display"

#include "VideoStitcher.hpp"
#include "ImageStitcher.hpp"
#include "Config.hpp"
#include "Utils.h"
#include "Timer.hpp"
#include "DShowUtility.h"

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;

void setAlphaNone(int state,void* param)
{
	if (state)
	{
		*((int*)param) = 0;
	}
}
void setAlphaConstant(int state,void* param)
{
	if (state)
	{
		*((int*)param) = 1;
	}
}
void setAlphaLinear(int state,void* param)
{
	if (state)
	{
		*((int*)param) = 2;
	}
}
void setAlphaExponential(int state,void* param)
{
	if (state)
	{
		*((int*)param) = 3;
	}
}
void setMaxTint(int state,void* param)
{
	*((bool*)param) = state;
}
void setInterpolation(int state,void* param)
{
	*((bool*)param) = state;
}

int runStitcHD()
{
	Config settings = Config::getConfig();

	VideoStitcher stitcher(settings);

	if (stitcher.start())
		return -1;
	
	cv::Mat frame;

	// Create control panel, etc
	createButton("Alpha Blend - None", setAlphaNone, &stitcher.config.alphaBlend, CV_RADIOBOX, (stitcher.config.alphaBlend == 0));
	createButton("Alpha Blend - Constant (Average)", setAlphaConstant, &stitcher.config.alphaBlend, CV_RADIOBOX, (stitcher.config.alphaBlend == 1));
	createButton("Alpha Blend - Linear", setAlphaLinear, &stitcher.config.alphaBlend, CV_RADIOBOX, (stitcher.config.alphaBlend == 2));
	createButton("Alpha Blend - Exponential", setAlphaExponential, &stitcher.config.alphaBlend, CV_RADIOBOX, (stitcher.config.alphaBlend == 3));

	createTrackbar("Blending", "", &stitcher.config.expBlendValue, 100);
	
	createButton("Interpolation", setInterpolation, &stitcher.config.interpolate, CV_CHECKBOX, stitcher.config.interpolate);
	
	createTrackbar("Set Tint", "", &stitcher.config.frameTint, 255);

	createButton("Use Maximum Tinting", setMaxTint, &stitcher.config.maxTint, CV_CHECKBOX, stitcher.config.maxTint);

	createTrackbar("Hmg Alpha", "", &stitcher.config.hmgTransitionAlpha, 100);
	createTrackbar("Overlap", "", &stitcher.config.frameOverlap, 100);
	createTrackbar("Hessian", "", &stitcher.config.hessianThreshold, 2000);

	char rc;

	while (true)
	{
		if (!stitcher.getImage())
			imshow(MAIN_WINDOW, stitcher.displayFrame);
        if((rc = waitKey(10)) >= 0) 
			break;
	}

	if (rc == ' ')
	{
		VideoWriter writer("OutputVideo.avi", CV_FOURCC('P','I','M','1'), 20, Size(800, 600));
		if (writer.isOpened())
		{
			// Run the same loop again
			while (true)
			{
				if (!stitcher.getImage())
				{
					imshow(MAIN_WINDOW, stitcher.displayFrame);
					writer << frame;
				}
				else cout << "Could not stitch." << endl;

				if((rc = waitKey(10)) >= 0) 
					break;
			}
		}
	}

	stitcher.stop();

	stitcher.config.writeToFile();
	stitcher.timer.writeToFile();

	destroyAllWindows();
	cout << endl;
	return 0;
}

int viewCameras()
{
	Config settings = Config::getConfig();

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

int mainMenu()
{
	string prompt = "Menu choice: ";
	while (true)
	{
		cout << "--- StitcHD Main Menu ---" << endl
			<< "1 - Run StitcHD" << endl
			<< "2 - View Cameras" << endl
			<< "3 - Quit" << endl << endl;

		cout << prompt;
		string inputStr;
		cin >> inputStr;
		int choice = atoi(inputStr.c_str());

		while (choice < 1 || choice > 10)
		{
			cout << "ERROR: Invalid input" << endl << endl;
			cout << prompt;
			cin >> inputStr;
			choice = atoi(inputStr.c_str());
		}

		if		(choice == 1)	runStitcHD();
		else if	(choice == 2)	viewCameras();
		else
			return 0;
	}
}

int main(int argc, char* argv[])
{
	return mainMenu();
}
