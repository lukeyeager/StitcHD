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

#include <vector>
#include <limits>
#include <ctime>
#include <iostream>

#include "ImageStitcher.hpp"
#include "Timer.hpp"
#include "Homographier.hpp"

#if COMPILE_GPU == 1
#include "GpuStitch.h"
#endif

using namespace cv;

Point ImageStitcher::applyHomographyToPoint(int x, int y, Mat &homography)
{
    Point point;
    double z = (homography.at<HOM_MAT_TYPE>(2, 0) * x + homography.at<HOM_MAT_TYPE>(2, 1) * y + homography.at<HOM_MAT_TYPE>(2, 2));
    double scale = 1./z;
    point.x = cvRound((homography.at<HOM_MAT_TYPE>(0, 0) * x + homography.at<HOM_MAT_TYPE>(0, 1) * y + homography.at<HOM_MAT_TYPE>(0, 2)) * scale);
    point.y = cvRound((homography.at<HOM_MAT_TYPE>(1, 0) * x + homography.at<HOM_MAT_TYPE>(1, 1) * y + homography.at<HOM_MAT_TYPE>(1, 2)) * scale);
    return point;
}

Point ImageStitcher::applyHomographyToPoint(Point point, Mat &homography)
{
    return applyHomographyToPoint(point.x, point.y, homography);
}

#if COMPILE_GPU == 1
Mat ImageStitcher::stitchImages_GPU(Mat* images, Mat* homographies, const Config& config)
{
	switch (config.camCount)
	{
	// We don't need to stitch if there's just one frame
	case 1:
		return images[0];

	// Stitch multiple
	case 2:
	case 3:
	case 4:
		break;
	default:
		cout << "Frame count not supported by ImageStitcher." << endl;
		return Mat(0,0,0);
	}

	vector<Mat> frames;
	vector<Mat> hmgs;

	for (int i=0; i<config.camCount; i++)
	{
		frames.push_back(images[i]);
	}

	// Create an identity matrix for the first image's transformation
	hmgs.push_back(Mat::eye(3, 3, homographies[0].type())); 

	if (config.camCount >= 2)
	{
		if (homographies[0].rows == 0 || homographies[0].cols == 0)
			return Mat(0,0,0);

		hmgs.push_back(homographies[0]);
	}

	if (config.camCount >= 3)
	{
		if (homographies[1].rows == 0 || homographies[1].cols == 0)
			return Mat(0,0,0);

		hmgs.push_back(homographies[1]);
	}

	if (config.camCount >= 4)
	{
		if (homographies[2].rows == 0 || homographies[2].cols == 0)
			return Mat(0,0,0);
		if (homographies[3].rows == 0 || homographies[3].cols == 0)
			return Mat(0,0,0);

		hmgs.push_back( (homographies[0] * homographies[2] + homographies[1] * homographies[3]) / 2.0f );
	}

	try
	{
		GpuStitch::StitchParams params;
		params.interpolate = config.interpolate;
		params.alphaBlend = config.alphaBlend;
		params.expBlendValue = config.expBlendValue;
		params.shift = config.frameTint;
		params.hardShift = config.maxTint;
		return GpuStitch::stitch_gpu(frames, hmgs, params);
	}
	catch (Exception)
	{
		return Mat(0,0,0);
	}
}
#endif

Mat ImageStitcher::stitchImages(Mat* images, Mat* homographies, const Config& config)
{
	switch (config.camCount)
	{
	// We don't need to stitch if there's just one frame
	case 1:
		return images[0];

	// Stitch two frames together
	case 2:
		return stitchTwoImages(images[0], images[1], homographies[0]);

	// Stitch four frames together
	case 4:
		return stitchFourImages(images, homographies);
	default:
		cout << "Frame count not supported by ImageStitcher." << endl;
		return Mat(0,0,0);
	}
}

Mat ImageStitcher::stitchTwoImages(Mat& image1, Mat& image2, Mat& homography)
{
	if (homography.rows == 0 || homography.cols == 0)
	{
		return Mat(0,0,0);
	}

	int shiftX = fabs(homography.at<HOM_MAT_TYPE>(0, 2)) * 2;
	int shiftY = fabs(homography.at<HOM_MAT_TYPE>(1, 2)) * 2;

	int cols = image1.cols + image2.cols;
	cols *= 2.5;
	int rows = image1.rows + image2.rows;
	rows *= 2.5;
	Mat canvas = Mat::zeros(Size(rows, cols), CV_8UC3);
	
	Vec3b image1Pixel(0, 0, 0);
	Vec3b image2Pixel(0, 0, 0);
	Vec3b combined;

	int minXColorPixel = canvas.cols - 1;
	int minYColorPixel = canvas.rows - 1;
	int maxXColorPixel = 0;
	int maxYColorPixel = 0;

	bool inImage1, inImage2;

	for (int r = 0; r < canvas.rows; r++) {
		for (int c = 0; c < canvas.cols; c++) {
			Point transformedPoint = applyHomographyToPoint(c - shiftX, r - shiftY, homography);

			inImage1 = true;
			inImage2 = true;

			if (c - shiftX < 0 || c - shiftX >= image1.cols
					|| r - shiftY < 0 || r - shiftY >= image1.rows)
				inImage1 = false;
			else
				image1Pixel = image1.at<Vec3b>(r - shiftY, c - shiftX);

			if (transformedPoint.x < 0 || transformedPoint.x >= image2.cols
					|| transformedPoint.y < 0 || transformedPoint.y >= image2.rows)
				inImage2 = false;
			else
				image2Pixel = image2.at<Vec3b>(transformedPoint.y, transformedPoint.x);

			if (inImage1 && inImage2)
			{
				combined = Vec3b(
					(image1Pixel[0] + image2Pixel[0])/2.0f,
					(image1Pixel[1] + image2Pixel[1])/2.0f,
					(image1Pixel[2] + image2Pixel[2])/2.0f );
			}
			else if (inImage1)
			{
				combined = image1Pixel;
			}
			else if (inImage2)
			{
				combined = image2Pixel;
			}
			else
			{
				combined = Vec3b(0,0,0);
			}

			if (!(combined[0]==0 && combined[1]==0 && combined[2]==0)) {
				if (c < minXColorPixel) {
					minXColorPixel = c;
				}

				if (c > maxXColorPixel) {
					maxXColorPixel = c;
				}

				if (r < minYColorPixel) {
					minYColorPixel = r;
				}

				if (r > maxYColorPixel) {
					maxYColorPixel = r;
				}
			}

			canvas.at<Vec3b>(r, c) = combined;
		}

	}

	if (minXColorPixel > 0)
		minXColorPixel--;
	else if (minXColorPixel < 0)
		minXColorPixel = 0;

	if (minYColorPixel > 0)
		minYColorPixel--;
	else if (minYColorPixel < 0)
		minYColorPixel = 0;

	if (maxXColorPixel < canvas.rows - 1)
		maxXColorPixel++;
	else if (maxXColorPixel > canvas.rows - 1)
		maxXColorPixel = canvas.rows - 1;

	if (maxYColorPixel < canvas.cols - 1)
		maxYColorPixel++;
	else if (maxYColorPixel > canvas.cols - 1)
		maxYColorPixel = canvas.cols - 1;

	if (minXColorPixel >= maxXColorPixel || minYColorPixel >= maxYColorPixel)
	{
		return Mat(0,0,0);
	}

	Mat output;
	canvas.colRange(minXColorPixel, maxXColorPixel).rowRange(minYColorPixel, maxYColorPixel).copyTo(output);
	
	return output;
}

Mat ImageStitcher::stitchFourImages(Mat* images, Mat* hmgs)
{
	// All the homographies must be valid
	if (hmgs[0].rows == 0 || hmgs[0].cols == 0)
		return Mat(0,0,0);
	if (hmgs[1].rows == 0 || hmgs[1].cols == 0)
		return Mat(0,0,0);
	if (hmgs[2].rows == 0 || hmgs[2].cols == 0)
		return Mat(0,0,0);

	Mat hmg1 = hmgs[0];
	Mat hmg2 = hmgs[1];
	Mat hmg3 = ((hmgs[0] * hmgs[2]) + (hmgs[1] * hmgs[3])) / 2.0;

	// Create the canvas
	int rows = max(images[0].rows, images[1].rows) + max(images[2].rows, images[3].rows);
	rows *= 2.5;
	int cols = max(images[0].cols, images[2].cols) + max(images[1].cols, images[3].cols);
	cols *= 2.5;
	Mat canvas = Mat::zeros(Size(rows, cols), CV_8UC3);
	
	Vec3b image0Pixel(0, 0, 0);
	Vec3b image1Pixel(0, 0, 0);
	Vec3b image2Pixel(0, 0, 0);
	Vec3b image3Pixel(0, 0, 0);
	Vec3b combined;

	int minXColorPixel = canvas.cols - 1;
	int minYColorPixel = canvas.rows - 1;
	int maxXColorPixel = 0;
	int maxYColorPixel = 0;

	bool inImage0, inImage1, inImage2, inImage3;

	for (int r = 0; r < canvas.rows; r++) {
		for (int c = 0; c < canvas.cols; c++) {
			Point image1point = applyHomographyToPoint(c, r, hmg1);
			Point image2point = applyHomographyToPoint(c, r, hmg2);
			Point image3point = applyHomographyToPoint(c, r, hmg3);
			
			inImage0 = true;
			inImage1 = true;
			inImage2 = true;
			inImage3 = true;

			// Image 0
			if (c < 0 || c >= images[0].cols
					|| r < 0 || r >= images[0].rows)
				inImage0 = false;
			else
				image0Pixel = images[0].at<Vec3b>(r, c);

			// Image 1
			if (image1point.x < 0 || image1point.x >= images[1].cols
					|| image1point.y < 0 || image1point.y >= images[1].rows)
				inImage1 = false;
			else
				image1Pixel = images[1].at<Vec3b>(image1point.y, image1point.x);
			
			// Image 2
			if (image2point.x < 0 || image2point.x >= images[2].cols
					|| image2point.y < 0 || image2point.y >= images[2].rows)
				inImage2 = false;
			else
				image2Pixel = images[2].at<Vec3b>(image2point.y, image2point.x);

			// Image 3
			if (image3point.x < 0 || image3point.x >= images[3].cols
					|| image3point.y < 0 || image3point.y >= images[3].rows)
				inImage3 = false;
			else
				image3Pixel = images[3].at<Vec3b>(image3point.y, image3point.x);

			combined = averagePixel(inImage0, inImage1, inImage2, inImage3, image0Pixel, image1Pixel, image2Pixel, image3Pixel);

			if (!(combined[0]==0 && combined[1]==0 && combined[2]==0)) {
				if (c < minXColorPixel) {
					minXColorPixel = c;
				}

				if (c > maxXColorPixel) {
					maxXColorPixel = c;
				}

				if (r < minYColorPixel) {
					minYColorPixel = r;
				}

				if (r > maxYColorPixel) {
					maxYColorPixel = r;
				}
			}

			canvas.at<Vec3b>(r, c) = combined;
		}

	}

	if (minXColorPixel > 0)
		minXColorPixel--;
	else if (minXColorPixel < 0)
		minXColorPixel = 0;

	if (minYColorPixel > 0)
		minYColorPixel--;
	else if (minYColorPixel < 0)
		minYColorPixel = 0;

	if (maxXColorPixel < canvas.rows - 1)
		maxXColorPixel++;
	else if (maxXColorPixel > canvas.rows - 1)
		maxXColorPixel = canvas.rows - 1;

	if (maxYColorPixel < canvas.cols - 1)
		maxYColorPixel++;
	else if (maxYColorPixel > canvas.cols - 1)
		maxYColorPixel = canvas.cols - 1;

	if (minXColorPixel >= maxXColorPixel || minYColorPixel >= maxYColorPixel)
	{
		return Mat(0,0,0);
	}

	Mat output;
	canvas.colRange(minXColorPixel, maxXColorPixel).rowRange(minYColorPixel, maxYColorPixel).copyTo(output);
	
	return output;

}

Vec3b ImageStitcher::averagePixel(bool b0, bool b1, bool b2, bool b3, Vec3b p0, Vec3b p1, Vec3b p2, Vec3b p3)
{
	if (b0 && b1 && b2 && b3)
		return Vec3b(
			(p0[0] + p1[0] + p2[0] + p3[0]) / 4.0f,
			(p0[1] + p1[1] + p2[1] + p3[1]) / 4.0f,
			(p0[2] + p1[2] + p2[2] + p3[2]) / 4.0f);
	else if (b0 && b1 && b2)
		return Vec3b(
			(p0[0] + p1[0] + p2[0]) / 3.0f,
			(p0[1] + p1[1] + p2[1]) / 3.0f,
			(p0[2] + p1[2] + p2[2]) / 3.0f);
	else if (b0 && b1 && b3)
		return Vec3b(
			(p0[0] + p1[0] + p3[0]) / 3.0f,
			(p0[1] + p1[1] + p3[1]) / 3.0f,
			(p0[2] + p1[2] + p3[2]) / 3.0f);
	else if (b0 && b2 && b3)
		return Vec3b(
			(p0[0] + p2[0] + p3[0]) / 3.0f,
			(p0[1] + p2[1] + p3[1]) / 3.0f,
			(p0[2] + p2[2] + p3[2]) / 3.0f);
	else if (b1 && b2 && b3)
		return Vec3b(
			(p1[0] + p2[0] + p3[0]) / 3.0f,
			(p1[1] + p2[1] + p3[1]) / 3.0f,
			(p1[2] + p2[2] + p3[2]) / 3.0f);
	else if (b0 && b1)
		return Vec3b(
			(p0[0] + p1[0]) / 2.0f,
			(p0[1] + p1[1]) / 2.0f,
			(p0[2] + p1[2]) / 2.0f);
	else if (b0 && b2)
		return Vec3b(
			(p0[0] + p2[0]) / 2.0f,
			(p0[1] + p2[1]) / 2.0f,
			(p0[2] + p2[2]) / 2.0f);
	else if (b0 && b3)
		return Vec3b(
			(p0[0] + p3[0]) / 2.0f,
			(p0[1] + p3[1]) / 2.0f,
			(p0[2] + p3[2]) / 2.0f);
	else if (b1 && b2)
		return Vec3b(
			(p1[0] + p2[0]) / 2.0f,
			(p1[1] + p2[1]) / 2.0f,
			(p1[2] + p2[2]) / 2.0f);
	else if (b1 && b3)
		return Vec3b(
			(p1[0] + p3[0]) / 2.0f,
			(p1[1] + p3[1]) / 2.0f,
			(p1[2] + p3[2]) / 2.0f);
	else if (b2 && b3)
		return Vec3b(
			(p2[0] + p3[0]) / 2.0f,
			(p2[1] + p3[1]) / 2.0f,
			(p2[2] + p3[2]) / 2.0f);
	else if (b0)
		return p0;
	else if (b1)
		return p1;
	else if (b2)
		return p2;
	else if (b3)
		return p3;
	else
		return Vec3b(0,0,0);
}
