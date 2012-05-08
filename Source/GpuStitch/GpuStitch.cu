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

#define CHANNELS 3
#define MAX_CAMERAS 4

#include "GpuStitch.h"

#include "math_functions.h"
#include "cuda_runtime.h"
#include "common_functions.h"

#include <limits>
#include <ctime>
using namespace std;

#include <iostream>
#include <opencv2/gpu/devmem2d.hpp>
#include <opencv2/gpu/gpumat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace cv::gpu;

namespace GpuStitch
{
	typedef unsigned char Tpixel;
	typedef float Thmg;

	const int MaxFrames = 4;

	// Used in device threads to transform a point using a homography
	__device__
	void applyHomographyToPoint(const int& x0, const int& y0,
		const DevMem2D_<Thmg>& hmgMem,
		float& x1, float& y1)
	{
		Thmg x, y, z;
		z = hmgMem.ptr(2)[0] * x0;
		z += hmgMem.ptr(2)[1] * y0;
		z += hmgMem.ptr(2)[2];

		x = hmgMem.ptr(0)[0] * x0;
		x += hmgMem.ptr(0)[1] * y0;
		x += hmgMem.ptr(0)[2];

		y = hmgMem.ptr(1)[0] * x0;
		y += hmgMem.ptr(1)[1] * y0;
		y += hmgMem.ptr(1)[2];

		x1 = x / z;
		y1 = y / z;
	}

	// Find the distance between two points
	__device__
	float getDistance(const int& x1, const int& y1,
		const int& x2, const int& y2)
	{
		return sqrtf( (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) );
	}

	// Used in interpolation to get the value of a Mat at a certain point
	__device__
	void getMatColorValue(const DevMem2D_<Tpixel>& src,
		const int& x, const int& y,
		int& val1, int& val2, int& val3)
	{
		if (x < 0 || x >= src.cols || y < 0 || y >= src.rows)
		{
			val1 = 0;
			val2 = 0;
			val3 = 0;
		}
		else
		{
			val1 = src.ptr(y)[x * CHANNELS];
			val2 = src.ptr(y)[x * CHANNELS + 1];
			val3 = src.ptr(y)[x * CHANNELS + 2];
		}
	}

	// Adds the value of a frame at a certain point on the canvas to the current RGB values at that point
	// Returns the multiplier for that point - depends on alpha blending
	__device__
	float addFrameToPixel(int& val1, int& val2, int& val3,
		const int& x, const int& y,
		const DevMem2D_<Tpixel>& src, const DevMem2D_<Thmg>& hmg,
		const StitchParams& params)
	{
		// Transform the pixel indices using the homography
		float tX, tY;
		applyHomographyToPoint(x, y, hmg, tX, tY);
		
		// Round them down to the nearest int
		int tXi = round(tX);
		int tYi = round(tY);

		if (!params.interpolate)
		{
			if (tXi < 1 || tXi >= src.cols || tYi < 1 || tYi >= src.rows)
				return 0;
		}
		else
		{
			if (tXi < 1 || tXi + 1 >= src.cols || tYi < 1 || tYi + 1 >= src.rows)
				return 0;
		}
		
		float rc;
		if (params.alphaBlend == 2)
		{
			// Linear blending
			rc = 1.0 - getDistance(tX, tY, src.cols / 2, src.rows / 2) / getDistance(0, 0, src.cols / 2, src.rows / 2);
		}
		else if (params.alphaBlend == 3)
		{
			// Exponential decay blending

			//		Get the Distance
			rc = getDistance(tX, tY, src.cols / 2, src.rows / 2) / getDistance(0, 0, src.cols / 2, src.rows / 2);
			//		Find the exponent
			rc = -(34.0 * params.expBlendValue + 100.0) * (rc - 0.5) / 50.0 - 1.0;
			//		Calculate the exponential
			rc = powf(10, rc);
		}
		else
		{
			// Average blending
			rc = 1.0;
		}

		if (!params.interpolate)
		{
			val1 += src.ptr(tYi)[tXi * CHANNELS];
			val2 += src.ptr(tYi)[tXi * CHANNELS + 1];
			val3 += src.ptr(tYi)[tXi * CHANNELS + 2];
			return rc;
		}
		else
		{
			// Bilinear interpolation

			tXi = floor(tX);
			tYi = floor(tY);

			float dX = tX - tXi;	// differential
			float dY = tY - tYi;

			int rgbValues[4][3];

			getMatColorValue(src, tXi, tYi,		rgbValues[0][0], rgbValues[0][1], rgbValues[0][2]);
			getMatColorValue(src, tXi+1, tYi,	rgbValues[1][0], rgbValues[1][1], rgbValues[1][2]);
			getMatColorValue(src, tXi, tYi+1,	rgbValues[2][0], rgbValues[2][1], rgbValues[2][2]);
			getMatColorValue(src, tXi+1, tYi+1,	rgbValues[3][0], rgbValues[3][1], rgbValues[3][2]);

			float x;

			// Update val1
			x = rgbValues[0][0] * (1-dX) * (1-dY) +
				rgbValues[1][0] * (dX) * (1-dY) +
				rgbValues[2][0] * (1-dX) * (dY) +
				rgbValues[3][0] * (dX) * (dY);
			if (x < 0)
				val1 = 0;
			else if (x > (unsigned char)(-1))
				val1 = (unsigned char)(-1);
			else
				val1 = x;

			// Update val2
			x = rgbValues[0][1] * (1-dX) * (1-dY) +
				rgbValues[1][1] * (dX) * (1-dY) +
				rgbValues[2][1] * (1-dX) * (dY) +
				rgbValues[3][1] * (dX) * (dY);
			if (x < 0)
				val2 = 0;
			else if (x > (unsigned char)(-1))
				val2 = (unsigned char)(-1);
			else
				val2 = x;

			// Update val3
			x = rgbValues[0][2] * (1-dX) * (1-dY) +
				rgbValues[1][2] * (dX) * (1-dY) +
				rgbValues[2][2] * (1-dX) * (dY) +
				rgbValues[3][2] * (dX) * (dY);
			if (x < 0)
				val3 = 0;
			else if (x > (unsigned char)(-1))
				val3 = (unsigned char)(-1);
			else
				val3 = x;

			return rc;
		}
	}

	// This is the entry point for the GPU - the kernel
	__global__
	void stitch_kernel(const int numFrames,
		DevMem2D_<Tpixel> const * const matSrc,
		DevMem2D_<Thmg> const * const matHmg,
		DevMem2D_<Tpixel> matDst,
		const StitchParams params)
	{
		int x = blockIdx.x * blockDim.x + threadIdx.x;
		int y = blockIdx.y * blockDim.y + threadIdx.y;
	
		if ((x < matDst.cols) && (y < matDst.rows))
		{
			int val1 = 0;	// RGB values
			int val2 = 0;
			int val3 = 0;

			unsigned char ucMax = (unsigned char)(-1);	// Comes in handy several times

			float multiplier = 0.0;
			float midV1=0, midV2=0, midV3=0;

			for (int i=0; i<numFrames; i++)
			{
				int v1=0, v2=0, v3=0;
				float m = addFrameToPixel(v1, v2, v3, x, y, matSrc[i], matHmg[i], params);

				if (m > 0.0)
				{
					if (params.hardShift)
					{
						switch (i)
						{
						case 0:
							v1 = ucMax; v2 = ucMax; v3 = ucMax;
							break;
						case 1:
							v1 = 0; v2 = 0; v3 = ucMax;
							break;
						case 2:
							v1 = 0; v2 = ucMax; v3 = 0;
							break;
						case 3:
							v1 = ucMax; v2 = 0; v3 = 0;
							break;
						default:
							break;	// We'll never get here
						}
					}
					else
					{
						if (i == 1) v3 += params.shift;
						if (i == 2) v2 += params.shift;
						if (i == 3) v1 += params.shift;
					}

					midV1 += m * v1;
					midV2 += m * v2;
					midV3 += m * v3;

					multiplier += m;
					//multiplier += 1.0;

					if (params.alphaBlend == 0)
						break;
				}
			}

			if (multiplier > 0.0)
			{
				val1 = int(midV1 / multiplier);
				val2 = int(midV2 / multiplier);
				val3 = int(midV3 / multiplier);

				if (val1 > ucMax)
					val1 = ucMax;
				if (val2 > ucMax)
					val2 = ucMax;
				if (val3 > ucMax)
					val3 = ucMax;
			}

			int pixelIdx = y * matDst.step + x * CHANNELS;
			matDst[pixelIdx] = val1;
			matDst[pixelIdx + 1] = val2;
			matDst[pixelIdx + 2] = val3;
		}
	}

	// Helpful function to check to see if a Cuda error has occurred
	__host__
	bool checkForCudaError(char* message)
	{
		cudaError_t error = cudaGetLastError();
		if (error != cudaSuccess) {
			cout << "ERROR (" << message << "): " << cudaGetErrorString(error) << endl;
			return true;
		}
		return false;
	}

	// Used in host thread to transform a point using a homography
	__host__
	Point applyHomographyToPoint(int x, int y, Mat &homography)
	{
		Point point;
		double z = (homography.at<double>(2, 0) * x + homography.at<double>(2, 1) * y + homography.at<double>(2, 2));
		double scale = 1./z;
		point.x = cvRound((homography.at<double>(0, 0) * x + homography.at<double>(0, 1) * y + homography.at<double>(0, 2)) * scale);
		point.y = cvRound((homography.at<double>(1, 0) * x + homography.at<double>(1, 1) * y + homography.at<double>(1, 2)) * scale);
		return point;
	}

	__host__
	Mat stitch_gpu(
		vector<Mat> matSrc,
		vector<Mat> matHmg,
		StitchParams params)
	{
		int numFrames = matSrc.size();

		if (numFrames < 2)
		{
			cout << "ERROR: Too few frames to stitch." << endl;
			return Mat(0,0,0);
		}
		if (numFrames > MaxFrames)
		{
			cout << "ERROR: Too many frames to stitch." << endl;
			return Mat(0,0,0);
		}
		if (numFrames != matHmg.size())
		{
			cout << "ERROR: Frames and Homographies don't match." << endl;
			return Mat(0,0,0);
		}
		for (int i=0; i<numFrames; i++)
		{
			if (matHmg[i].rows == 0 || matHmg[i].cols == 0)
				return Mat(0,0,0);
		}

		int minX = 0;
		int minY = 0; 
		int maxX = matSrc[0].cols;
		int maxY = matSrc[0].rows;

		for (int i=1; i<matSrc.size(); i++)
		{
			Mat inv = matHmg[i].inv();
			Point p = applyHomographyToPoint(0, 0, inv);
			if (p.x > maxX)		maxX = p.x;
			if (p.x < minX)		minX = p.x;
			if (p.y > maxY)		maxY = p.y;
			if (p.y < minY)		minY = p.y;
			p = applyHomographyToPoint(0, matSrc[i].rows - 1, inv);
			if (p.x > maxX)		maxX = p.x;
			if (p.x < minX)		minX = p.x;
			if (p.y > maxY)		maxY = p.y;
			if (p.y < minY)		minY = p.y;
			p = applyHomographyToPoint(matSrc[i].cols - 1, 0, inv);
			if (p.x > maxX)		maxX = p.x;
			if (p.x < minX)		minX = p.x;
			if (p.y > maxY)		maxY = p.y;
			if (p.y < minY)		minY = p.y;
			p = applyHomographyToPoint(matSrc[i].cols - 1, matSrc[i].rows - 1, inv);
			if (p.x > maxX)		maxX = p.x;
			if (p.x < minX)		minX = p.x;
			if (p.y > maxY)		maxY = p.y;
			if (p.y < minY)		minY = p.y;
		}

		int offsetX = matSrc[0].cols - (maxX + minX) / 2;
		int offsetY = matSrc[0].rows - (maxY + minY) / 2;

		Mat translation = (Mat_<double>(3,3) << 1, 0, -offsetX, 0, 1, -offsetY, 0, 0, 1);

		for(int i=0; i<matSrc.size(); i++)
			matHmg[i] = matHmg[i] * translation;

		GpuMat matDstDev = GpuMat(2 * matSrc[0].rows, 2 * matSrc[0].cols, CV_8UC3);

		vector<GpuMat> matSrcDev(MaxFrames);
		vector<GpuMat> matHmgDev(MaxFrames);

		DevMem2D_<Tpixel> matSrcMem_h[MAX_CAMERAS];
		DevMem2D_<Thmg> matHmgMem_h[MAX_CAMERAS];

		// Convert Mats into GpuMats and then DevMem2Ds
		for (int i=0; i<numFrames; i++)
		{
			matSrcDev[i] = GpuMat(matSrc[i]);
			Mat tmp;
			matHmg[i].convertTo(tmp, CV_32FC1);
			matHmgDev[i] = GpuMat(tmp);

			matSrcMem_h[i] = matSrcDev[i];
			matHmgMem_h[i] = matHmgDev[i];
		}

		void* matSrcMem_d;
		cudaMalloc(&matSrcMem_d, sizeof(matSrcMem_h));

		if (checkForCudaError("cudaMalloc"))
			return Mat(0,0,0);

		void* matHmgMem_d;
		cudaMalloc(&matHmgMem_d, sizeof(matHmgMem_h));

		if (checkForCudaError("cudaMalloc"))
		{
			cudaFree(matHmgMem_d);
			return Mat(0,0,0);
		}

		cudaMemcpy(matSrcMem_d, matSrcMem_h, sizeof(matSrcMem_h), cudaMemcpyHostToDevice);

		if (checkForCudaError("cudaMemcpy"))
		{
			cudaFree(matSrcMem_d);
			cudaFree(matHmgMem_d);
			return Mat(0,0,0);
		}

		cudaMemcpy(matHmgMem_d, matHmgMem_h, sizeof(matHmgMem_h), cudaMemcpyHostToDevice);

		if (checkForCudaError("cudaMemcpy"))
		{
			cudaFree(matSrcMem_d);
			cudaFree(matHmgMem_d);
			return Mat(0,0,0);
		}

		dim3 block(32, 16, 1);

		int x = int(0.5f + float(matDstDev.cols * CHANNELS) / float(block.x));
		int y = int(0.5f + float(matDstDev.rows) / float(block.y));
		dim3 grid(x, y, 1);

		stitch_kernel<<<grid, block>>>(
			numFrames,
			(DevMem2D_<Tpixel>*)matSrcMem_d,
			(DevMem2D_<Thmg>*)matHmgMem_d,
			matDstDev,
			params);

		cudaDeviceSynchronize();
		bool rc = checkForCudaError("after kernel launch");
		
		cudaFree(matSrcMem_d);
		cudaFree(matHmgMem_d);

		if (rc)
			return Mat(0,0,0);
		else
		{
			Mat output;
			matDstDev.download(output);
			return output;
		}
	}
}
