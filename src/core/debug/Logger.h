#pragma once

#include <fstream>
#include <sstream>

class Logger;

#define LOG(level, message)                             \
	do                                                    \
	{                                                     \
		std::stringstream ss;                               \
		ss << message;                                      \
		Logger::print(level, ss.str(), __FILE__, __LINE__); \
	} while (false)

enum LogLevel
{
	ERROR = 1,
	WARN,
	INFO,
	ALL,
};

class Logger
{
public:
	static bool init(std::string filename, LogLevel level);
	static void print(LogLevel level, std::string message, const char *file, int line);
	static bool destroy();

private:
	static std::ofstream m_file;
	static int m_level;
};