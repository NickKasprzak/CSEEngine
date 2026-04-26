#pragma once
#include "../FileNavigator.h"

#if WIN32
#include <Windows.h>
#include <fileapi.h>

#ifdef CreateFile
#undef CreateFile
#endif

#ifdef DeleteFile
#undef DeleteFile
#endif

#endif

namespace CSECore
{

class FileNavigator_Win32 : public FileNavigator
{
public:
	FileNavigator_Win32();
	~FileNavigator_Win32() override;

	Expected<std::shared_ptr<File>, std::string> CreateFile(std::string filepath, FileAccessMode accessMode, bool overwriteExisting) override;
	Expected<std::shared_ptr<File>, std::string> OpenFile(std::string filepath, FileAccessMode accessMode, bool openAlways) override;
	bool DeleteFile(std::string filepath) override;

	bool FindFile(std::string filepath) override;
	bool FindDirectory(std::string directory) override;

private:

};

typedef FileNavigator_Win32 FileNavigator_Device;

}