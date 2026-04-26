#include "Logger.h"
#include <iostream>
#include <fstream>

namespace CSECore
{

enum LogLevel
{
	LOG_LEVEL_VERBOSE,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
};

struct Log
{
	std::string message;
	LogLevel level;

	Log(LogLevel level, std::string message);
	Log(const Log& other);
	Log(Log&& other) noexcept;
	~Log();

	Log& operator=(const Log& other);
	Log& operator=(Log&& other) noexcept;
};

Log::Log(LogLevel level, std::string message)
	: message(message), level(level)
{

}

Log::Log(const Log& other)
	: message(other.message), level(other.level)
{

}

Log::Log(Log&& other) noexcept
	: message(std::move(other.message)), level(other.level)
{

}

Log::~Log()
{

}

Log& Log::operator=(const Log& other)
{
	message = other.message;
	level = other.level;
	return *this;
}

Log& Log::operator=(Log&& other) noexcept
{
	message = std::move(other.message);
	level = other.level;
	return *this;
}

class Logger
{
public:
	Logger();
	~Logger();

	static Logger& Instance();

	void SetLogOutput(std::string filename);
	void LogMessage(Log loggedMessage);
	void FlushMessages();

private:
	std::ofstream _fileStream;
};

Logger::Logger()
	: _fileStream()
{

}

Logger::~Logger()
{

}

Logger& Logger::Instance()
{
	static Logger instance;
	return instance;
}

void Logger::SetLogOutput(std::string filename)
{
	_fileStream.open(filename);
}

void Logger::LogMessage(Log loggedMessage)
{
	_fileStream << loggedMessage.message;
	std::cout << loggedMessage.message << '\n';
}

void Logger::FlushMessages()
{
	_fileStream.flush();
}

void LogVerbose(std::stringstream message)
{
	Logger::Instance().LogMessage(Log(LOG_LEVEL_VERBOSE, message.str()));
}

void LogInfo(std::stringstream message)
{
	Logger::Instance().LogMessage(Log(LOG_LEVEL_INFO, message.str()));
}

void LogWarning(std::stringstream message)
{
	Logger::Instance().LogMessage(Log(LOG_LEVEL_WARNING, message.str()));
}

void LogError(std::stringstream message)
{
	Logger::Instance().LogMessage(Log(LOG_LEVEL_ERROR, message.str()));
}

void LogSetOutputFile(std::string filepath)
{
	Logger::Instance().SetLogOutput(filepath);
}

void LogFlush()
{
	Logger::Instance().FlushMessages();
}

}