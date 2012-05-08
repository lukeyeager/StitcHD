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

#ifndef STITCHD_CONFIG_HPP
#define STITCHD_CONFIG_HPP 1

#define COMPILE_GPU 1
#define MAX_CAMERAS 4

#include <string>
#include <vector>
using namespace std;

class __declspec(dllexport) Config
{
public:

	// Static constants
	static const string FileName;

	// Member attributes

	// Changeable only at program start
	int camCount;
	bool camInverted[MAX_CAMERAS];
	int camSizes[MAX_CAMERAS][2];
	bool showFps;

	// Related to stitcher
	bool interpolate;
	int alphaBlend;
	int expBlendValue;
	int frameTint;
	bool maxTint;

	// Related to homographiers
	int hmgCount;
	int hmgTargets[MAX_CAMERAS][2];
	char hmgDirections[MAX_CAMERAS][2];
	bool showMatches;
	int frameOverlap;			// 0 - 100 %
	int hmgTransitionAlpha;		// 0 - 100 %
	int hessianThreshold;
	int nOctaves;				//default: 4
	int nOctaveLayers;			//def: 2
	bool extended;				//def: false
	bool upright;				//def: false
	int flannMatchOpt;			//def: 1, 0=bruteforce, 1=kdtree, 2=autotuned
	int flannChecks;			//def: 32 (SearchParams value, used in knnSearch)
	int flannTrees;				//def: 4, range 1 - 16
	int flannTargetPrecision;	//def: 0.9, 0 - 100 %, specifies accuracy of search
	int flannBuildWeight;		//def: 0.01, 0 - 100 %, importance of index build time relative to search time
	int flannMemoryWeight;		//def: 0, divided by 100.0, tradeoff between time and memory 
								//	(value / 100.0 < 1.0 => time is important, value / 100.0 > 1 => memory is important)
	int flannSampleFraction;	//default 0.1, 0 - 100 %, range 0.0 - 1.0, how much of dataset to use
	int matchTolerance;			//default 0.5, 0 - 100 %, range 0.0 - 1.0
	int ransacReprojThresh;		//default 3, divided by 10.0, typical ranges for val / 10.0 => 1 to 10

	// Constructor	
	Config();

	// Member functions
	void setDefaults();
	int readFromFile();
	int writeToFile();
	void print(ostream& os) const;

	static Config getConfig();

private:
	
	int readInputLine(string);

	// ----- Obsolete with QT GUI -----

	// Used in getConfig()
	void identifyCameras();
	void changeCamCount();
	void changeCamSize();
	void changeCamInverted();
	void changeShowMatches();
	void changeFrameOverlap();
	void changeInterpolate();
	void changeAlphaBlend();

};

#endif