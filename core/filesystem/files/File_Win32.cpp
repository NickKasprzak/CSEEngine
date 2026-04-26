#include "File_Win32.h"
#include "../../CSEAssert.h"
#include <algorithm>

namespace CSECore
{

File_Win32::File_Win32(HANDLE fileHandle, FileAccessMode accessMode)
	: _fileHandle(fileHandle), _accessMode(accessMode)
{

}

File_Win32::~File_Win32()
{
	CloseHandle(_fileHandle);
}

size_t File_Win32::Read(void* buffer, size_t bufferSize, size_t amount)
{
	CSE_ASSERT((_accessMode == ACCESS_READ) || (_accessMode == ACCESS_READ_WRITE), "Attempted to read from file without proper access mode.");

	DWORD amountToRead = (std::min)(bufferSize, amount);
	DWORD amountRead = 0;
	BOOL result = ReadFile(_fileHandle, buffer, amountToRead, &amountRead, NULL);

	if (!result)
	{
		return 0;
	}

	return (size_t)amountRead;
}

size_t File_Win32::Write(void* buffer, size_t bufferSize, size_t amount)
{
	CSE_ASSERT((_accessMode == ACCESS_WRITE) || (_accessMode == ACCESS_READ_WRITE), "Attempted to write to file without proper access mode.");

	DWORD amountToWrite = (std::min)(bufferSize, amount);
	DWORD amountWritten = 0;
	BOOL result = WriteFile(_fileHandle, buffer, amountToWrite, &amountWritten, NULL);

	if (!result)
	{
		return 0;
	}

	return (size_t)amountWritten;
}

size_t File_Win32::Seek(size_t position)
{
	LONG posLow = position & UINT32_MAX;
	LONG posHigh = (position >> (sizeof(LONG) * 8)); // this is wrong.
	return (size_t)SetFilePointer(_fileHandle, posLow, &posHigh, FILE_BEGIN);
}

size_t File_Win32::SeekBegin()
{
	LONG posLow = 0;
	return (size_t)SetFilePointer(_fileHandle, posLow, NULL, FILE_BEGIN);
}

size_t File_Win32::SeekEnd()
{
	LONG posLow = 0;
	return (size_t)SetFilePointer(_fileHandle, posLow, NULL, FILE_END);
}

size_t File_Win32::Tell()
{
	LONG posLow = 0;
	return (size_t)SetFilePointer(_fileHandle, posLow, NULL, FILE_CURRENT);
}

bool File_Win32::Flush()
{
	return FlushFileBuffers(_fileHandle);
}

}