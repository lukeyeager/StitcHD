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

#include "Homographier.hpp"
#include "Config.hpp"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
using namespace std;

#include <opencv2/flann/flann.hpp>
#include <opencv2/flann/all_indices.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
using namespace cv;

#if COMPILE_GPU == 1
#include <opencv2/gpu/gpumat.hpp>
#include <opencv2/gpu/gpu.hpp>
using namespace cv::gpu;
#endif

Homographier::Homographier(int id,
		const Config& c,
		HANDLE startEvent,
		HANDLE stopEvent,
		char hmgDirectionA,
		char hmgDirectionB)
	:id(id),
		config(c),
		startEvent(startEvent),
		stopEvent(stopEvent)
{
	running = false;
	threadHandle = INVALID_HANDLE_VALUE;
	doneEvent = NULL;
	homography = Mat::eye(3, 3, CV_64FC1);
	matchesFrame = Mat(0,0,0);
	maskA = Mat();
	maskB = Mat();
	hmgDirections[0] = hmgDirectionA;
	hmgDirections[1] = hmgDirectionB;
}

Homographier::~Homographier()
{
	if (running)
		stop();

	if (doneEvent != NULL)
	{
		CloseHandle(doneEvent);
	}
}

int Homographier::start()
{
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
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("CreateEvent returned a duplicate handle.\n");
		return -1;
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
		cout << "Could not start Homographier " << id << " thread." << endl;
		running = false;
		return -1;
	}
	
	return 0;
}

int Homographier::stop()
{
	if (!running)
		return 0;

	running = false;
	DWORD returnCode;
	
	// This forces all other Homographier threads to close too
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

int Homographier::run()
{
	cout << "Started Homographier " << id << '.' << endl;
	running = true;

	DWORD waitResult;
	do
	{
		waitResult = WaitForSingleObject( 
				startEvent,		// event handle
				2000);			// time-out

		switch (waitResult) 
		{
			// The thread got ownership of the mutex
			case WAIT_OBJECT_0:
				break; 

			case WAIT_TIMEOUT:
				printf("Homographier wait for startEvent timed out.\n");
				SetEvent(doneEvent);
				continue;

			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
			default:
				printf("Error in Homographier while waiting for startEvent: (%d)\n", GetLastError()); 
				return -1; 
		}
		
		if (frameA.cols > 0 && frameA.rows > 0 &&
				frameB.cols > 0 && frameB.rows > 0)
		{
			try
			{
				Mat newH = findHomography(frameA, frameB);

				// Take average of new and old
				if (newH.cols > 0 && newH.rows > 0)
				{
					float alpha = float(config.hmgTransitionAlpha) / 100.0;
					homography = (newH * alpha + homography * (1.0 - alpha) );
				}
			}
			catch (Exception &e)
			{
				std::cout << "ERROR: " << e.msg << std::endl;
			}
		}

		SetEvent(doneEvent);

		// Wait for stopEvent
		waitResult = WaitForSingleObject( 
			stopEvent,			// event handle
			5000);				// timeout
			
		switch (waitResult) 
		{
			// Event object was signaled
			case WAIT_OBJECT_0:
				break; 

			case WAIT_TIMEOUT:
				printf("Homographier wait for stopEvent timed out.\n");
				SetEvent(doneEvent);
				continue;

			// An error occurred
			default: 
				printf("Error in CaptureThread while waiting for stopEvent: (%d)\n", GetLastError()); 
				return -1;
		}

	} while (running);

	cout << "Ending Homographier " << id << " thread." << endl;
	running = false;
	return 0;
}

#if COMPILE_GPU == 1
Mat Homographier::findHomography_GPU(Mat &image1, Mat &image2)
{
	Timer::send(Timer::Homography, id, Timer::HmgTimeval::Start);

    Mat gray1 = mat2Grayscale(image1);
    Mat gray2 = mat2Grayscale(image2);

	GpuMat gray_gpu1(gray1);
	GpuMat gray_gpu2(gray2);

	SURF_GPU gpu_surfer(config.hessianThreshold, config.nOctaves, config.nOctaveLayers, 
		config.extended);

	maskA = Mat::ones(gray1.rows, gray1.cols, CV_8UC1);
	maskB = Mat::ones(gray2.rows, gray2.cols, CV_8UC1);

	float overlap = float(config.frameOverlap) / 100.0;
	//set mask for first image
	switch(hmgDirections[0])
	{
	case 'R':	//the ROI is on the right of the image (set left side to 0)
		maskA.colRange(0, (maskA.cols-1) * (1 - overlap)).rowRange(0, maskA.rows-1).setTo(0);
		break;
	case 'L':	//ROI is to the left
		maskA.colRange((maskA.cols-1) * overlap, maskA.cols-1).rowRange(0, maskA.rows-1).setTo(0);
		break;
	case 'D':	//ROI is down/bottom half
		maskA.rowRange(0, (maskA.rows-1) * (1 - overlap)).colRange(0, maskA.cols-1).setTo(0);
		break;
	case 'U':	//ROI is up/top half
		maskA.rowRange((maskA.rows-1) * overlap, maskA.rows-1).colRange(0, maskA.cols-1).setTo(0);
		break;
	default:
		break;
	}

	//set mask for second image
	switch(hmgDirections[1])
	{
	case 'R':	//the ROI is on the right of the image (set left side to 0)
		maskB.colRange(0, (maskB.cols-1) * (1 - overlap)).rowRange(0, maskB.rows-1).setTo(0);
		break;
	case 'L':	//ROI is to the left
		maskB.colRange((maskB.cols-1) * overlap, maskB.cols-1).rowRange(0, maskB.rows-1).setTo(0);
		break;
	case 'D':	//ROI is down/bottom half
		maskB.rowRange(0, (maskB.rows-1) * (1 - overlap)).colRange(0, maskB.cols-1).setTo(0);
		break;
	case 'U':	//ROI is up/top half
		maskB.rowRange((maskB.rows-1) * overlap, maskB.rows-1).colRange(0, maskB.cols-1).setTo(0);
		break;
	default:
		break;
	}

	GpuMat mask_gpuA(maskA);
	GpuMat mask_gpuB(maskB);

	GpuMat keypoints1GPU, keypoints2GPU;
    GpuMat descriptors1GPU, descriptors2GPU;
    gpu_surfer(gray_gpu1, mask_gpuA, keypoints1GPU, descriptors1GPU);
    gpu_surfer(gray_gpu2, mask_gpuB, keypoints2GPU, descriptors2GPU);

	Timer::send(Timer::Homography, id, Timer::HmgTimeval::Detect);

	// matching descriptors
    BruteForceMatcher_GPU< L2<float> > matcher;
    GpuMat trainIdx, distance;
    matcher.matchSingle(descriptors1GPU, descriptors2GPU, trainIdx, distance);

	Timer::send(Timer::Homography, id, Timer::HmgTimeval::Match);
    
    // downloading results
    vector<KeyPoint> keypoints1, keypoints2;
    vector<float> descriptors1, descriptors2;
    vector<DMatch> matches, good_matches;
    gpu_surfer.downloadKeypoints(keypoints1GPU, keypoints1);
    gpu_surfer.downloadKeypoints(keypoints2GPU, keypoints2);
    gpu_surfer.downloadDescriptors(descriptors1GPU, descriptors1);
    gpu_surfer.downloadDescriptors(descriptors2GPU, descriptors2);
    BruteForceMatcher_GPU< L2<float> >::matchDownload(trainIdx, distance, matches);

	if (keypoints1.size() == 0 || keypoints2.size() == 0)
	{
		return Mat(0,0,0);
	}

	double total_dist = 0;
	double min_dist = matches[0].distance;
	double max_dist = matches[0].distance;
	double avg_dist, swing; 
	double tolerance = float(config.matchTolerance) / 100.0;

	for(int i=0; i<matches.size(); i++)
	{
		double dist = matches[i].distance;

		if(dist < min_dist)
			min_dist = dist;
		if(dist > max_dist)
			max_dist = dist;
		total_dist += dist;
	}

	avg_dist = total_dist / (double)matches.size();

	if( (avg_dist - min_dist) > (max_dist - avg_dist))
		swing = (avg_dist - min_dist) * tolerance;
	else
		swing = (max_dist - avg_dist) * tolerance;

	for (int i=0; i<matches.size(); i++)
	{
		if((avg_dist - swing <= matches[i].distance) 
			&& (matches[i].distance <= avg_dist + swing))
		{
			good_matches.push_back(matches[i]);
		}
	}

	vector<Point2f> image1Points, image2Points;
	for (int i=0; i<matches.size(); i++)
	{
		image1Points.push_back(keypoints1[good_matches[i].queryIdx].pt);
		image2Points.push_back(keypoints2[good_matches[i].trainIdx].pt);
	}

    Mat homography = cv::findHomography(image1Points, image2Points, CV_RANSAC,
		float(config.ransacReprojThresh) / 10.0);

	Timer::send(Timer::Homography, id, Timer::HmgTimeval::End);

	if (config.showMatches)
	{
		drawMatches(gray1, keypoints1, gray2, keypoints2, good_matches, matchesFrame);

		// Draw descriptive keypoints:
		//drawMatches(gray1, keypoints1, gray2, keypoints2, good_matches, matchesFrame, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS | DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	}

    return homography;
}
#endif

//CPU Version
Mat Homographier::findHomography(Mat &image1, Mat &image2)
{
	Timer::send(Timer::Homography, id, Timer::HmgTimeval::Start);

    Mat gray1 = mat2Grayscale(image1);
    Mat gray2 = mat2Grayscale(image2);

	//Construct SURF detection object w/ (hessianThreshold, nOctaves=4, nOctaveLayers=2, extended=F, upright=F)
	//Specifically, upright=true provides a speedboost when cameras aren't rotated with respect to each other
	cv::SURF surfer(config.hessianThreshold, config.nOctaves, config.nOctaveLayers, 
		config.extended, config.upright);
	
	// detecting keypoints and computing descriptors (this used to be split into separate tasks)
	vector<KeyPoint> keypoints1, keypoints2;
	vector<float> cv_descriptors1, cv_descriptors2;
	Mat descriptors1, descriptors2;

	//if(maskA.cols != gray1.cols || maskA.rows != gray1.rows ||
	//	maskB.cols != gray2.cols || maskB.rows != gray2.rows)
	//{
	

	maskA = Mat::ones(gray1.rows, gray1.cols, CV_8UC1);
	maskB = Mat::ones(gray2.rows, gray2.cols, CV_8UC1);

	float overlap = float(config.frameOverlap) / 100.0;
	//set mask for first image
	switch(hmgDirections[0])
	{
	case 'R':	//the ROI is on the right of the image (set left side to 0)
		maskA.colRange(0, (maskA.cols-1) * (1 - overlap)).rowRange(0, maskA.rows-1).setTo(0);
		break;
	case 'L':	//ROI is to the left
		maskA.colRange((maskA.cols-1) * overlap, maskA.cols-1).rowRange(0, maskA.rows-1).setTo(0);
		break;
	case 'D':	//ROI is down/bottom half
		maskA.rowRange(0, (maskA.rows-1) * (1 - overlap)).colRange(0, maskA.cols-1).setTo(0);
		break;
	case 'U':	//ROI is up/top half
		maskA.rowRange((maskA.rows-1) * overlap, maskA.rows-1).colRange(0, maskA.cols-1).setTo(0);
		break;
	default:
		break;
	}

	//set mask for second image
	switch(hmgDirections[1])
	{
	case 'R':	//the ROI is on the right of the image (set left side to 0)
		maskB.colRange(0, (maskB.cols-1) * (1 - overlap)).rowRange(0, maskB.rows-1).setTo(0);
		break;
	case 'L':	//ROI is to the left
		maskB.colRange((maskB.cols-1) * overlap, maskB.cols-1).rowRange(0, maskB.rows-1).setTo(0);
		break;
	case 'D':	//ROI is down/bottom half
		maskB.rowRange(0, (maskB.rows-1) * (1 - overlap)).colRange(0, maskB.cols-1).setTo(0);
		break;
	case 'U':	//ROI is up/top half
		maskB.rowRange((maskB.rows-1) * overlap, maskB.rows-1).colRange(0, maskB.cols-1).setTo(0);
		break;
	default:
		break;
	}

	//}

	surfer(gray1, maskA, keypoints1, cv_descriptors1, false);
	
	// Copy 1d vector data to 2d cv::Mat
	descriptors1.create(keypoints1.size(), surfer.descriptorSize(), CV_32FC1);
	assert( (int)cv_descriptors1.size() == descriptors1.rows * descriptors1.cols );
	std::copy(cv_descriptors1.begin(), cv_descriptors1.end(), descriptors1.begin<float>());

	surfer(gray2, maskB, keypoints2, cv_descriptors2, false);

	descriptors2.create(keypoints2.size(), surfer.descriptorSize(), CV_32FC1);
	assert( (int)cv_descriptors2.size() == descriptors2.rows * descriptors2.cols );
	std::copy(cv_descriptors2.begin(), cv_descriptors2.end(), descriptors2.begin<float>());

	Timer::send(Timer::Homography, id, Timer::HmgTimeval::Detect);
	
	if (keypoints1.size() == 0 || keypoints2.size() == 0)
	{
		return Mat(0,0,0);
	}


	// matching descriptors
	FlannBasedMatcher matcher;

	switch(config.flannMatchOpt)
	{
	case 0:
		matcher = FlannBasedMatcher(new flann::LinearIndexParams, new flann::SearchParams(config.flannChecks));
		break;
	case 1:
		matcher = FlannBasedMatcher(new flann::KDTreeIndexParams(config.flannTrees), 
			new flann::SearchParams(config.flannChecks));
		break;
	case 2:
		matcher = FlannBasedMatcher(new flann::AutotunedIndexParams(
			float(config.flannTargetPrecision) / 100.0,
			float(config.flannBuildWeight) / 100.0, 
			float(config.flannMemoryWeight) / 100.0, 
			float(config.flannSampleFraction) / 100.0),
			new flann::SearchParams(config.flannChecks));
		break;
	default:
		break;
	}
	//BruteForceMatcher<L2<float> > matcher;
	//FlannBasedMatcher matcher;
	vector<DMatch> matches, good_matches;

	matcher.match(descriptors1, descriptors2, matches);

	Timer::send(Timer::Homography, id, Timer::HmgTimeval::Match);

	double total_dist = 0;
	double min_dist = matches[0].distance;
	double max_dist = matches[0].distance;
	double avg_dist, swing; 
	double tolerance = float(config.matchTolerance) / 100.0;

	for(int i=0; i<matches.size(); i++)
	{
		double dist = matches[i].distance;

		if(dist < min_dist)
			min_dist = dist;
		if(dist > max_dist)
			max_dist = dist;
		total_dist += dist;
	}

	avg_dist = total_dist / (double)matches.size();

	if( (avg_dist - min_dist) > (max_dist - avg_dist))
		swing = (avg_dist - min_dist) * tolerance;
	else
		swing = (max_dist - avg_dist) * tolerance;

	for (int i=0; i<matches.size(); i++)
	{
		if((avg_dist - swing <= matches[i].distance) 
			&& (matches[i].distance <= avg_dist + swing))
		{
			good_matches.push_back(matches[i]);
		}
	}

	vector<Point2f> image1Points, image2Points;
	for (int i=0; i<good_matches.size(); i++)
	{
		image1Points.push_back(keypoints1[good_matches[i].queryIdx].pt);
		image2Points.push_back(keypoints2[good_matches[i].trainIdx].pt);
	}

    Mat homography = cv::findHomography(image1Points, image2Points, CV_RANSAC, 
		float(config.ransacReprojThresh) / 10.0);

	Timer::send(Timer::Homography, id, Timer::HmgTimeval::End);

	if (config.showMatches)
	{
		drawMatches(gray1, keypoints1, gray2, keypoints2, good_matches, matchesFrame);

		// Draw descriptive keypoints:
		//drawMatches(gray1, keypoints1, gray2, keypoints2, good_matches, matchesFrame, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS | DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	}

    return homography;
}

Mat Homographier::mat2Grayscale(Mat& image)
{
	Mat grayscale;
	cvtColor(image, grayscale, CV_RGB2GRAY);
    return grayscale;
}

void Homographier::printHomography(Mat& h)
{
	int width = 15;
	cout << setw(width) << h.at<HOM_MAT_TYPE>(0, 0)
		<< setw(width) << h.at<HOM_MAT_TYPE>(0, 1)
		<< setw(width) << h.at<HOM_MAT_TYPE>(0, 2) << endl
		<< setw(width) << h.at<HOM_MAT_TYPE>(1, 0)
		<< setw(width) << h.at<HOM_MAT_TYPE>(1, 1)
		<< setw(width) << h.at<HOM_MAT_TYPE>(1, 2) << endl
		<< setw(width) << h.at<HOM_MAT_TYPE>(2, 0)
		<< setw(width) << h.at<HOM_MAT_TYPE>(2, 1)
		<< setw(width) << h.at<HOM_MAT_TYPE>(2, 2) << endl << endl;
}
