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

#ifndef IMAGESTITCHER_H
#define IMAGESTITCHER_H

#include "Config.hpp"
#include "Timer.hpp"

#include <vector>
using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

class ImageStitcher {
public:

	/// Stitch camCount images together
    static Mat stitchImages(Mat* images, Mat* homographies, const Config& config);
	
#if COMPILE_GPU == 1
    static Mat stitchImages_GPU(Mat* images, Mat* homographies, const Config& config);
#endif

	/// Stitch two images together
	static Mat stitchTwoImages(Mat& image1, Mat& image2, Mat& homography);

	/// Stitch four images together
	static Mat stitchFourImages(Mat* images, Mat* homographies);

private:

    static Point applyHomographyToPoint(int, int, Mat &homography);
    static Point applyHomographyToPoint(Point point, Mat &homography);

	static Vec3b averagePixel(bool, bool, bool, bool, Vec3b, Vec3b, Vec3b, Vec3b);
};

#endif // IMAGESTITCHER_H
