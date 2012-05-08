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

#ifndef TIMER_HPP
#define TIMER_HPP

#include "Config.hpp"

#include <Windows.h>
#include <vector>
#include <ctime>
#include <iostream>
using namespace std;

class __declspec(dllexport) Timer
{
public:
	
	static enum Type
	{
		Camera,
		Homography,
		Stitch
	};
	
	Timer(const Config c);

	~Timer()
	{
		if (running)
			stop();
	}

	int start();
	int stop();
	static int send(Type type, int id, int subType);

	void print(ostream& os);
	void writeToFile();

	static int msTime(const clock_t, const clock_t);
	
	// Stores timing information about the Homographier
	struct CamTimeval
	{
		static enum Type
		{
			Start,
			End
		};

		CamTimeval()
			:times(Type::End + 1)	// Initialize size
		{ }

		vector<clock_t> times;
	};

	// Stores timing information about the Homographier
	struct HmgTimeval
	{
		static enum Type
		{
			Start,
			Detect,
			Match,
			End
		};

		HmgTimeval()
			:times(Type::End + 1)	// Initialize size
		{ }

		vector<clock_t> times;
	};

	// Stores timing information about the Homographier
	struct StitchTimeval
	{
		static enum Type
		{
			Start,
			End
		};

		StitchTimeval()
			:times(Type::End + 1)	// Initialize size
		{ }

		vector<clock_t> times;
	};

private:

	const Config config;

	static const int Port;
	static const int BufferSize;

	bool running; // This is only true while the Timer is running
	HANDLE threadHandle;

	vector<vector<CamTimeval>> camTimevals;
	vector<vector<HmgTimeval>> hmgTimevals;
	vector<StitchTimeval> stitchTimevals;

	// Thread entry point
	static DWORD WINAPI StartThread(LPVOID arg)
	{
		return ((Timer*)arg)->run();
	}

	// Thread function
	int run();
};

#endif