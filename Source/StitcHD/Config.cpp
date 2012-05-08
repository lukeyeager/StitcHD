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

#include "Config.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

// Set static const members
const string Config::FileName = "../StitcHD_Config.txt";

Config::Config()
{
	setDefaults();
}

void Config::setDefaults()
{
	camCount = 4;
	hmgCount = 4;
	
	int width = 800;
	int height = 600;
	int defaultSizes[MAX_CAMERAS][2] = { {width,height}, {width,height}, {width,height}, {width,height} };

	showFps = true;
	
	int defaultTargets[MAX_CAMERAS][2] = { {0,1}, {0,2}, {1,3}, {2,3} };
	char defaultDirections[MAX_CAMERAS][2] = { {'R', 'L'}, {'U', 'D'}, {'U', 'D'}, {'R', 'L'} };

	for (int i=0; i<MAX_CAMERAS; i++)
	{
		camSizes[i][0] = defaultSizes[i][0];
		camSizes[i][1] = defaultSizes[i][1];
		hmgTargets[i][0] = defaultTargets[i][0];
		hmgTargets[i][1] = defaultTargets[i][1];
		hmgDirections[i][0] = defaultDirections[i][0];
		hmgDirections[i][1] = defaultDirections[i][1];
		camInverted[i] = false;
	}

	interpolate = true;
	alphaBlend = 3;
	expBlendValue = 50;
	frameTint = 0;
	maxTint = false;

	showMatches = false;
	frameOverlap = 80;
	hmgTransitionAlpha = 20;
	hessianThreshold = 500;

	nOctaves = 4;
	nOctaveLayers = 4;
	extended = false;
	upright = false;
	flannMatchOpt = 1;
	flannChecks = 32;
	flannTrees = 4;
	flannTargetPrecision = 90;
	flannBuildWeight = 1;
	flannMemoryWeight = 0;
	flannSampleFraction = 10;
	matchTolerance = 50;
	ransacReprojThresh = 30;
}

int Config::readFromFile()
{
	string str;
	istringstream ss;
	ifstream file(FileName);
	if (file.is_open())
	{
		getline(file, str);	// Header

		while (!file.eof())
		{
			getline(file, str);
			readInputLine(str
				);
		}

		file.close();
		return 0;
	}
	else
	{
		cout << "Unable to open file" << endl;
		return -1;
	}
}

int Config::readInputLine(string line)
{
	istringstream iss(line);
	string type;
	iss >> type;

	if (type == "CamCount:")
	{
		string str;
		iss >> str;
		int x = atoi(str.c_str());
		if (x >= 1 && x <= MAX_CAMERAS)
			camCount = x;
	}
	else if (type == "CamWidths:")
	{
		for (int i = 0; i<MAX_CAMERAS; i++)
		{
			string str;
			iss >> str;
			int w = atoi(str.c_str());
			if (w >= 100 && w <= 1600)
				camSizes[i][0] = w;
		}
	}
	else if (type == "CamHeights:")
	{
		for (int i = 0; i<MAX_CAMERAS; i++)
		{
			string str;
			iss >> str;
			int h = atoi(str.c_str());
			if (h >= 100 || h <= 1200)
				camSizes[i][1] = h;
		}
	}
	else if (type == "CamInverted:")
	{
		for (int i = 0; i<MAX_CAMERAS; i++)
		{
			string str;
			iss >> str;
			int inv = atoi(str.c_str());
			if (inv == 0 || inv == 1)
				camInverted[i] = (bool)inv;
		}
	}
	else if (type == "HmgCount:")
	{
		string str;
		iss >> str;
		int count = atoi(str.c_str());
		if (count >= 0 && count <= camCount)
			hmgCount = count;
	}
	else if (type == "HmgTargets:")
	{
		for (int i = 0; i<MAX_CAMERAS; i++)
			for (int j=0; j<2; j++)
			{
				string str;
				iss >> str;
				int t = atoi(str.c_str());
				if (str.size() == 1 && t >= 0 && t < MAX_CAMERAS)
					hmgTargets[i][j] = t;
			}
	}
	else if (type == "HmgDirections:")
	{
		for (int i = 0; i<MAX_CAMERAS; i++)
			for (int j=0; j<2; j++)
			{
				string str;
				iss >> str;
				if (str.size() == 1 && (str[0] == 'U' || str[0] == 'D' || str[0] == 'L' || str[0] == 'R'))
					hmgDirections[i][j] = str[0];
			}
	}
	else if (type == "FrameOverlap:")
	{
		string str;
		iss >> str;
		int overlap = atof(str.c_str());
		if (overlap > 0 && overlap <= 100)
			frameOverlap = overlap;
	}
	else if (type == "HmgTransitionAlpha:")
	{
		string str;
		iss >> str;
		int alpha = atoi(str.c_str());
		if (alpha >= 0 && alpha <= 100)
			hmgTransitionAlpha = alpha;
	}
	else if (type == "HessianThreshold:")
	{
		string str;
		iss >> str;
		int thold = atoi(str.c_str());
		if (thold >= 0 && thold <= 2000)
			hessianThreshold = thold;
	}
	else if (type == "Interpolate:")
	{
		string str;
		iss >> str;
		int result = atoi(str.c_str());
		if (result == 0 || result == 1)
			interpolate = (bool)result;
	}
	else if (type == "AlphaBlend:")
	{
		string str;
		iss >> str;
		int result = atoi(str.c_str());
		if (result >= 0 && result <= 3)
			alphaBlend = result;
	}
	else if (type == "ExpBlendValue:")
	{
		string str;
		iss >> str;
		int blend = atoi(str.c_str());
		if (blend >= 0 && blend <= 100)
			expBlendValue = blend;
	}
	else if (type == "nOctaves:")
	{
		string str;
		iss >> str;
		int octaves = atoi(str.c_str());
		if (octaves > 0)
			nOctaves = octaves;
	}
	else if (type == "nOctaveLayers:")
	{
		string str;
		iss >> str;
		int layers = atoi(str.c_str());
		if (layers > 0)
			nOctaveLayers = layers;
	}
	else if (type == "extended:")
	{
		string str;
		iss >> str;
		int result = atoi(str.c_str());
		if (result == 0 || result == 1)
			extended = (bool)result;
	}
	else if (type == "upright:")
	{
		string str;
		iss >> str;
		int result = atoi(str.c_str());
		if (result == 0 || result == 1)
			upright = (bool)result;
	}
	else if (type == "flannMatchOpt:")
	{
		string str;
		iss >> str;
		int option = atoi(str.c_str());
		if (option >= 0 && option <= 2)
			flannMatchOpt = option;
	}
	else if (type == "flannChecks:")
	{
		string str;
		iss >> str;
		int checks = atoi(str.c_str());
		if (checks > 0)
			flannChecks = checks;
	}
	else if (type == "flannTrees:")
	{
		string str;
		iss >> str;
		int trees = atoi(str.c_str());
		if (trees >= 1 && trees <= 16)
			flannTrees = trees;
	}
	else if (type == "flannTargetPrecision:")
	{
		string str;
		iss >> str;
		int precision = atoi(str.c_str());
		if (precision >= 0 && precision <= 100)
			flannTargetPrecision = precision;
	}
	else if (type == "flannBuildWeight:")
	{
		string str;
		iss >> str;
		int weight = atoi(str.c_str());
		if (weight >= 0 && weight <= 100)
			flannBuildWeight = weight;
	}
	else if (type == "flannMemoryWeight:")
	{
		string str;
		iss >> str;
		int weight = atoi(str.c_str());
		//can range anywhere above or below 1
			flannMemoryWeight = weight;
	}
	else if (type == "flannSampleFraction:")
	{
		string str;
		iss >> str;
		int frac = atoi(str.c_str());
		if (frac >= 0 && frac <= 100)
			flannSampleFraction = frac;
	}
	else if (type == "matchTolerance:")
	{
		string str;
		iss >> str;
		int tolerance = atoi(str.c_str());
		if (tolerance >= 0 && tolerance <= 100)
			matchTolerance = tolerance;
	}
	else if (type == "ransacReprojThresh:")
	{
		string str;
		iss >> str;
		int threshold = atoi(str.c_str());
		if (threshold >= 10 && threshold <= 150)
			ransacReprojThresh = threshold;
	}
	return 0;
}

int Config::writeToFile()
{
	ofstream file(FileName);
	if (file.is_open())
	{
		file << "--- StitcHD Config File ---" << endl;

		file << "CamCount: " << camCount << endl;
		file << "CamWidths:";
		for (int i=0; i<MAX_CAMERAS; i++)
			file << ' ' << camSizes[i][0];
		file << endl;
		file << "CamHeights:";
		for (int i=0; i<MAX_CAMERAS; i++)
			file << ' ' << camSizes[i][1];
		file << endl;
		file << "CamInverted:";
		for (int i=0; i<MAX_CAMERAS; i++)
			file << ' ' << camInverted[i];
		file << endl;
		
		file << "HmgCount: " << hmgCount << endl;
		file << "HmgTargets:";
		for (int i=0; i<MAX_CAMERAS; i++)
			file << ' ' << hmgTargets[i][0] << ' ' << hmgTargets[i][1];
		file << endl;

		file << "HmgDirections:";
		for (int i=0; i<MAX_CAMERAS; i++)
			file << ' ' << hmgDirections[i][0] << ' ' << hmgDirections[i][1];
		file << endl;

		file << "FrameOverlap: " << frameOverlap << endl;

		file << "HmgTransitionAlpha: " << hmgTransitionAlpha << endl;
		file << "HessianThreshold: " << hessianThreshold << endl;
		file << "nOctaves: " << nOctaves << endl;
		file << "nOctaveLayers: " << nOctaveLayers << endl;
		file << "extended: " << extended << endl;
		file << "upright: " << upright << endl;

		file << "flannMatchOpt: " << flannMatchOpt << endl;
		file << "flannChecks: " << flannChecks << endl;
		file << "flannTrees: " << flannTrees << endl;
		file << "flannTargetPrecision: " << flannTargetPrecision << endl;
		file << "flannBuildWeight: " << flannBuildWeight << endl;
		file << "flannMemoryWeight: " << flannMemoryWeight << endl;
		file << "flannSampleFraction: " << flannSampleFraction << endl;

		file << "matchTolerance: " << matchTolerance << endl;
		file << "ransacReprojThresh: " << ransacReprojThresh << endl;

		file << "AlphaBlend: " << alphaBlend << endl;
		file << "ExpBlendValue: " << expBlendValue << endl;

		file.close();
		return 0;
	}
	else
	{
		cout << "Unable to open file" << endl;
		return -1;
	}
}

void Config::print(ostream& os) const
{
	os << "Number of Cameras:\t" << camCount << endl;
	for (int i=0; i<camCount; i++)
	{
		os << "\t" << i+1 << ": " << camSizes[i][0] << 'x' << camSizes[i][1];
		if (camInverted[i])
			os << " - Inverted";
		os << endl;
	}

	// Stitcher
	os << endl;
	os << "Interpolate: " << interpolate << endl;
	os << "Alpha Blending: ";
	switch (alphaBlend)
	{
	case 0: os << "Overlay"; break;
	case 1: os << "Average"; break;
	case 2: os << "Linear"; break;
	case 3: os << "Exponential, " << expBlendValue << '%'; break;
	default: os << "<ERROR>"; break;
	}
	os << endl;

	// Homographier
	os << endl;
	os << "Number of homographiers: " << hmgCount << endl;
	os << "% Frame Overlap: " << frameOverlap << endl;
	os << "Transition Alpha: " << hmgTransitionAlpha << '%' << endl;
	os << "Hessian Threshold: " << hessianThreshold << endl;
	os << "nOctaves: " << nOctaves << endl;
	os << "nOctaveLayers: " << nOctaveLayers << endl;
	os << "Extended: " << extended << endl;
	os << "Upright: " << upright << endl;
	os << "Matcher type: ";
	switch (flannMatchOpt)
	{
	case 0: os << "Brute Force"; break;
	case 1: os << "KD Tree"; break;
	case 2: os << "Autotuned"; break;
	default: os << "<ERROR>"; break;
	}
	os << endl;

	// etc
	os << "Match Tolerance: " << matchTolerance << '%' << endl;
	os << "RANSAC Threshold: " << ransacReprojThresh << endl;

	os << endl;
}

// ----- Obsolete with QT GUI -----

Config Config::getConfig()
{
	Config config;

	if (0 != config.readFromFile())
		cout << "No config file exists, using default values." << endl;

	bool done = false;

	while (!done)
	{
		cout << endl << "<<<Current config>>>" <<endl;
		config.print(cout);

		cout << endl << "--- Configuration Menu ---" << endl
			<< "1 - Accept Settings" << endl
			<< "2 - Change Camera Count" << endl
			<< "3 - Change Camera Resolutions" << endl
			<< "4 - Change Camera Inversions" << endl
			<< "5 - Change Show Homography Matches" << endl
			<< "6 - Use Default Settings" << endl;
		
		bool failed = false;
		int choice;
		do
		{
			if (failed)
				cout << "ERROR: Invalid input" << endl << endl;

			cout << "Menu choice: ";
			string inputStr;
			cin >> inputStr;
			choice = atoi(inputStr.c_str());

			// Read until we get a valid input
			if (choice < 1 || choice > 10)
				failed = true;
			else
				failed = false;
		} while (failed);

		if (choice == 1)	done = true;
		else if (choice == 2)	config.changeCamCount();
		else if (choice == 3)	config.changeCamSize();
		else if (choice == 4)	config.changeCamInverted();
		else if (choice == 5)	config.changeShowMatches();
		else if (choice == 6)	config.setDefaults();

	}

	config.writeToFile();

	return config;
}

void Config::changeCamCount()
{
	bool failed = false;
	int newCount = 0;
	
	do
	{
		if (failed)
			cout << "ERROR: Not supported." << endl << endl;

		cout << "Number of cameras: ";
		string inputStr;
		cin >> inputStr;
		int x = atoi(inputStr.c_str());

		// Read until we get a valid input
		if (x >= 1 && x <= MAX_CAMERAS)
		{
			newCount = x;
			failed = false;
		}
		else
			failed = true;

	} while (failed);

	camCount = newCount;

	switch (camCount)
	{
	case 1:
		hmgCount = 0;
		break;
	case 2:
		hmgCount = 1;
		break;
	case 3:
		hmgCount = 2;
		break;
	case 4:
		hmgCount = 4;
		break;
	default:
		break;
	}
}

void Config::changeCamSize()
{
	int whichCam = -1;
	bool failed;

	if (camCount == 1)
		whichCam = 0;	// All
	else
	{
		failed = false;
		do
		{
			if (failed)
				cout << "ERROR: Invalid input." << endl << endl;

			cout << "Which camera (1-" << camCount << ", or \"a\" for all cameras): ";
			string inputStr;
			cin >> inputStr;
			if (inputStr[0] == 'a' || inputStr[0] == 'A')
				whichCam = 0;
			else
			{
				int x = atoi(inputStr.c_str());

				// Read until we get a valid input
				if (x < 1 || x > camCount)
					failed = true;
				else
				{
					whichCam = x;
					failed = false;
				}
			}
		} while (failed);
	}

	int width;
	failed = false;
	do
	{
		if (failed)
			cout << "ERROR: Not supported." << endl;

		cout << "Camera width: ";
		string inputStr;
		cin >> inputStr;
		width = atoi(inputStr.c_str());

		// Read until we get a valid input
		failed = (width <= 0 || width > 1600);
	} while (failed);

	int height;
	failed = false;
	do
	{
		if (failed)
			cout << "ERROR: Not supported." << endl;

		cout << "Camera height: ";
		string inputStr;
		cin >> inputStr;
		height = atoi(inputStr.c_str());

		// Read until we get a valid input
		failed = (height <= 0 || height > 1600);
	} while (failed);

	if (whichCam == 0)
	{
		for (int i=0; i<camCount; i++)
		{
			camSizes[i][0] = width;
			camSizes[i][1] = height;
		}
	}
	else
	{
		camSizes[whichCam-1][0] = width;
		camSizes[whichCam-1][1] = height;
	}
}

void Config::changeCamInverted()
{
	int whichCam = -1;
	bool failed;

	if (camCount == 1)
		whichCam = 0;	// All
	else
	{
		failed = false;
		do
		{
			if (failed)
				cout << "ERROR: Invalid input." << endl << endl;

			cout << "Which camera (1-" << camCount << "): ";
			string inputStr;
			cin >> inputStr;

			int x = atoi(inputStr.c_str());
			// Read until we get a valid input
			if (x < 1 || x > camCount)
				failed = true;
			else
			{
				whichCam = x;
				failed = false;
			}
		} while (failed);
	}

	bool inv;
	failed = false;
	do
	{
		if (failed)
			cout << "ERROR: Invalid input." << endl;

		cout << "Inverted (y/n): ";
		string inputStr;
		cin >> inputStr;

		if (inputStr[0] == 'y' || inputStr[0] == 'Y')
		{
			inv = true;
			failed = false;
		}
		else if (inputStr[0] == 'n' || inputStr[0] == 'N')
		{
			inv = false;
			failed = false;
		}
		else
			failed = true;
	} while (failed);
	
	camInverted[whichCam-1] = inv;
}

void Config::changeFrameOverlap()
{
	bool failed = false;
	
	do
	{
		if (failed)
			cout << "ERROR: Not supported." << endl << endl;

		cout << "Frame overlap fraction (0.0 - 1.0): ";
		string inputStr;
		cin >> inputStr;
		frameOverlap = atof(inputStr.c_str());

		// Read until we get a valid input
		if (frameOverlap <= 0 || frameOverlap > 1)
			failed = true;
		else
			failed = false;
	} while (failed);
}

void Config::changeInterpolate()
{
	bool failed = false;
	
	do
	{
		if (failed)
			cout << "ERROR: Invalid input" << endl << endl;

		cout << "Use interpolation (y/n): ";
		string inputStr;
		cin >> inputStr;

		// Read until we get a valid input
		if (inputStr[0] == 'y' || inputStr[0] == 'Y')
		{
			interpolate = true;
			failed = false;
		}
		else if (inputStr[0] == 'n' || inputStr[0] == 'N')
		{
			interpolate = false;
			failed = false;
		}
		else
			failed = true;
	} while (failed);
}

void Config::changeAlphaBlend()
{
	bool failed = false;
	int choice;
	
	do
	{
		if (failed)
			cout << "ERROR: Invalid input" << endl << endl;

		cout << "Alpha Blending Types" << endl
			<< "1 - None (Overlay)" << endl
			<< "2 - Constant (Average)" << endl
			<< "3 - Linear" << endl
			<< "4 - Exponential" <<endl
			<< endl
			<< "Type: ";
		string inputStr;
		cin >> inputStr;
		choice = atoi(inputStr.c_str());

		// Read until we get a valid input
		failed = (choice < 1 || choice > 4);
	} while (failed);
	
	alphaBlend = choice - 1;
}

void Config::changeShowMatches()
{
	bool failed = false;
	
	do
	{
		if (failed)
			cout << "ERROR: Invalid input" << endl << endl;

		cout << "Show homography matches (y/n): ";
		string inputStr;
		cin >> inputStr;

		// Read until we get a valid input
		if (inputStr[0] == 'y' || inputStr[0] == 'Y')
		{
			showMatches = true;
			failed = false;
		}
		else if (inputStr[0] == 'n' || inputStr[0] == 'N')
		{
			showMatches = false;
			failed = false;
		}
		else
			failed = true;
	} while (failed);
}
