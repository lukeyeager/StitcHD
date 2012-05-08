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

#ifndef HOMOGRAPHIER_HPP
#define HOMOGRAPHIER_HPP

#include "Config.hpp"
#include "Timer.hpp"

#include <Windows.h>
#include <opencv2/core/core.hpp>
using namespace cv;

typedef double HOM_MAT_TYPE;

class Homographier
{
public:

	int id;
	Config config;
	HANDLE doneEvent;
	Mat homography;
	Mat frameA, frameB;
	Mat maskA, maskB;
	Mat matchesFrame;
	char hmgDirections[2];
	
	// Constructor
	Homographier(int, const Config&, HANDLE, HANDLE, char, char);

	~Homographier();

	int start();
	int stop();
	
	// Find a homography using the CPU
	Mat findHomography(Mat &image1, Mat &image2);

#if COMPILE_GPU == 1
	// Find a homography using the GPU
	Mat findHomography_GPU(Mat &image1, Mat &image2);
#endif

private:
	
	bool running;
	HANDLE startEvent, stopEvent, threadHandle;

	// Thread entry point
	static DWORD WINAPI StartThread(LPVOID arg)
	{
		return ((Homographier*)arg)->run();
	}

	// Thread function
	int run();

	// Convert a Mat into grayscale
	static Mat mat2Grayscale(Mat &image);

	// For debugging 
	void printHomography(Mat &h);
};

#endif