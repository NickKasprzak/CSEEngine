#pragma once
#include <string>
#include <sstream>

namespace CSECore
{

void LogVerbose(std::stringstream message);
void LogInfo(std::stringstream message);
void LogWarning(std::stringstream message);
void LogError(std::stringstream message);

// Just have this accept a file handle to whatever file should be logged to
void LogSetOutputFile(std::string filepath);
void LogFlush();

#define CSE_LOGV(inputStream)\
{\
	CSECore::LogVerbose(std::stringstream() << inputStream);\
}

#define CSE_LOGI(inputStream)\
{\
	CSECore::LogInfo(std::stringstream() << inputStream);\
}

#define CSE_LOGW(inputStream)\
{\
	CSECore::LogWarning(std::stringstream() << inputStream);\
}

#define CSE_LOGE(inputStream)\
{\
	CSECore::LogError(std::stringstream() << inputStream);\
}

}