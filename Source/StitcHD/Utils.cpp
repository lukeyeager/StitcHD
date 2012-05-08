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

#include "Utils.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
using namespace boost::posix_time;
using namespace boost::filesystem;

#include <iostream>
using namespace std;

string timerOutputFileName()
{
	ptime now = second_clock::local_time();

	path path("..\\TimerData\\");

	if (exists(path))
	{
		if (!is_directory(path))
		{
			cout << "ERROR: " << path.string() << " is a file." << endl;
			return "";
		}
	}
	else
	{
		if (!create_directory(path))
		{
			cout << "ERROR: Could not create directory " << path.string() << endl;
			return "";
		}
	}

	path /= to_simple_string(now.date());

	if (exists(path))
	{
		if (!is_directory(path))
		{
			cout << "ERROR: " << path.string() << " is a file." << endl;
			return "";
		}
	}
	else
	{
		if (!create_directory(path))
		{
			cout << "ERROR: Could not create directory " << path.string() << endl;
			return "";
		}
	}

	path /= to_iso_string(now.time_of_day()) + ".txt";

	return path.string();
}

string videoOutputFileName()
{
	ptime now = second_clock::local_time();

	path path("..\\Videos\\");

	if (exists(path))
	{
		if (!is_directory(path))
		{
			cout << "ERROR: " << path.string() << " is a file." << endl;
			return "";
		}
	}
	else
	{
		if (!create_directory(path))
		{
			cout << "ERROR: Could not create directory " << path.string() << endl;
			return "";
		}
	}

	path /= to_simple_string(now.date());

	if (exists(path))
	{
		if (!is_directory(path))
		{
			cout << "ERROR: " << path.string() << " is a file." << endl;
			return "";
		}
	}
	else
	{
		if (!create_directory(path))
		{
			cout << "ERROR: Could not create directory " << path.string() << endl;
			return "";
		}
	}

	path /= to_iso_string(now.time_of_day()) + ".mpeg";

	return path.string();
}

string pictureOutputFileName()
{
	ptime now = second_clock::local_time();

	path path("..\\Pictures\\");

	if (exists(path))
	{
		if (!is_directory(path))
		{
			cout << "ERROR: " << path.string() << " is a file." << endl;
			return "";
		}
	}
	else
	{
		if (!create_directory(path))
		{
			cout << "ERROR: Could not create directory " << path.string() << endl;
			return "";
		}
	}

	path /= to_simple_string(now.date());

	if (exists(path))
	{
		if (!is_directory(path))
		{
			cout << "ERROR: " << path.string() << " is a file." << endl;
			return "";
		}
	}
	else
	{
		if (!create_directory(path))
		{
			cout << "ERROR: Could not create directory " << path.string() << endl;
			return "";
		}
	}

	path /= to_iso_string(now.time_of_day()) + ".png";

	return path.string();
}