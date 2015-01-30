/*
 * Logger.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: hudo
 */

#include "debug/Logger.h"
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <string>  // string

std::ofstream Logger::m_file;
int Logger::m_level;

bool Logger::init(std::string filename, LogLevel level)
{
	m_level = level;
	m_file.open(filename.c_str());
	if(m_file.is_open())
	{
		return true;
	}

	return false;
}

bool Logger::destroy()
{
	m_file.close();
	return true;
}


const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

void Logger::print(LogLevel level, std::string message, const char* file, int line)
{
	if(!m_file.is_open() || level > m_level)
		return;
	m_file << currentDateTime();
	switch(level)
	{
		case INFO:
		{
			m_file<<" [INFO] ";
			break;
		}
		case WARN:
		{
			m_file<<" [WARN] ";
			break;
		}
		case ERROR:
		{
			m_file<<" [ERROR] ";
			break;
		}
		default:
		{
			m_file<<" [INFO] ";
			break;
		}
	}
	m_file<<"- "<<file<<":"<<line<<" - "<<message<<std::endl;
}




