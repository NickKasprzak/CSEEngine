#pragma once
#include "FileNavigator.h"
#include "File.h"
#include "FileAccessModes.h"
#include "../Expected.h"
#include <string>
#include <memory>

namespace CSECore
{

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	template<typename NavigatorType>
	void SetNavigator();

	Expected<std::shared_ptr<File>, std::string> CreateFile(std::string filepath, FileAccessMode accessMode, bool overwriteExisting = false);
	Expected<std::shared_ptr<File>, std::string> OpenFile(std::string filepath, FileAccessMode accessMode, bool openAlways = false);
	bool DeleteFile(std::string filepath);
	bool FindFile(std::string filepath);

private:
	FileNavigator* _navigator;
};

template<typename NavigatorType>
void FileSystem::SetNavigator()
{
	_navigator = new NavigatorType();
}

}