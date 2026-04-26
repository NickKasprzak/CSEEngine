#pragma once
#include "File.h"
#include "FileAccessModes.h"
#include "../Expected.h"
#include <string>
#include <memory>

namespace CSECore
{

class FileNavigator
{
public:
	virtual ~FileNavigator() {};

	virtual Expected<std::shared_ptr<File>, std::string> CreateFile(std::string filepath, FileAccessMode accessMode, bool overwriteExisting) = 0;
	virtual Expected<std::shared_ptr<File>, std::string> OpenFile(std::string filepath, FileAccessMode accessMode, bool openAlways) = 0;
	virtual bool DeleteFile(std::string filepath) = 0;

	virtual bool FindFile(std::string filepath) = 0;
	virtual bool FindDirectory(std::string directory) = 0;
};

}