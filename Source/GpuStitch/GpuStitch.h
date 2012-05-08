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

#ifndef GPU_STITCH_HPP
#define GPU_STITCH_HPP 1

#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>

#include "cuda_runtime.h"

using namespace std;
using namespace cv;

namespace GpuStitch
{
	__declspec(dllexport)
	struct StitchParams
	{
		StitchParams()
		{
			interpolate = true;
			alphaBlend = 2;
			expBlendValue = 50;
			shift = 0;
			hardShift = false;
		}

		bool interpolate;
		int alphaBlend;
		float expBlendValue;
		int shift;
		bool hardShift;
	};

	__declspec(dllexport)
	Mat stitch_gpu(
		vector<Mat> matSrc,
		vector<Mat> matHmg,
		StitchParams params
		);
}

#endif