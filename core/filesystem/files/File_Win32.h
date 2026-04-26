#pragma once
#include "../File.h"
#include "../FileAccessModes.h"

#if WIN32
#include <Windows.h>
#include <fileapi.h>
#endif

namespace CSECore
{

class File_Win32 : public File
{
public:
	File_Win32(HANDLE fileHandle, FileAccessMode accessMode);
	~File_Win32() override;

	size_t Read(void* buffer, size_t bufferSize, size_t amount) override;
	size_t Write(void* buffer, size_t bufferSize, size_t amount) override;

	size_t Seek(size_t position) override;
	size_t SeekBegin() override;
	size_t SeekEnd() override;

	size_t Tell() override;

	bool Flush() override;

private:
	HANDLE _fileHandle;
	FileAccessMode _accessMode;
};

}