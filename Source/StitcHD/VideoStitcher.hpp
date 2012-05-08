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

#ifndef VIDEOSTITCHER_HPP
#define VIDEOSTITCHER_HPP

#include <vector>

#include <opencv2/highgui/highgui.hpp>

#include "Timer.hpp"
#include "ImageStitcher.hpp"
#include "Homographier.hpp"
#include "CameraCapture.hpp"
#include "Config.hpp"

using namespace cv;

class __declspec(dllexport) VideoStitcher {
public:	

	// Keeps timing information
	Timer timer;

	// Stores most variable information about this stitcher
	Config& config;

	// The most recently stitched frame
	Mat displayFrame;

	// The latency of the homographierController
	int hmgLatency;

	// Constructor
	VideoStitcher(Config&);

	// Destructor
	~VideoStitcher();

	static int ViewCameras(Config&);

	int start();
	int stop();
	int closeMatchFrames();
	int startRecording();
	int stopRecording();

	// Run the VideoStitcher
	int getImage();

	int showImage();

private:

	bool running;

	bool recording;
	VideoWriter *recorder;

	// Objects for the CaptureThreads
	vector<CameraCapture*> cameraCaptures;
	HANDLE startCapEvent, stopCapEvent;
	HANDLE framesMutex;
	
	// Objects for controlling the Homographiers
	vector<Homographier*> homographiers;
	bool hmgCntlRunning;
	HANDLE hmgCntlThreadHandle;
	HANDLE startHmgEvent, stopHmgEvent;

	// Stitches images together
	ImageStitcher imageStitcher;
	
	// Called from within start()
	int startCameraCaptures();
	int startHmgController();

	// Thread entry point
	static DWORD WINAPI StartHmgController(LPVOID arg)
	{
		return ((VideoStitcher*)arg)->runHmgController();
	}

	// Thread function
	int runHmgController();
	int stopHmgController();
};

#endif // VIDEOSTITCHER_HPP
