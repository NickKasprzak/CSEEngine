#include "FileNavigator_Win32.h"
#include "../files/File_Win32.h"

namespace CSECore
{

std::string IsolateDirectory(std::string filepath)
{
	size_t dirEnd = filepath.find_last_of('/');

	if (dirEnd == std::string::npos)
	{
		return std::string();
	}

	return filepath.substr(0, dirEnd + 1);
}

DWORD GetWin32AccessMode(FileAccessMode accessMode)
{
	switch (accessMode)
	{
	case ACCESS_READ:
		return GENERIC_READ;
		break;
	case ACCESS_WRITE:
		return GENERIC_WRITE;
		break;
	case ACCESS_READ_WRITE:
		return GENERIC_READ | GENERIC_WRITE;
		break;
	default:
		return 0;
		break;
	}
}

DWORD GetWin32ShareMode(FileAccessMode accessMode)
{
	switch (accessMode)
	{
	case ACCESS_READ:
		return FILE_SHARE_READ;
		break;
	case ACCESS_WRITE:
		return FILE_SHARE_WRITE;
		break;
	case ACCESS_READ_WRITE:
		return FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;
	default:
		return 0;
		break;
	}
}

std::string TranslateWin32ErrorCode(DWORD errorCode)
{
	switch (errorCode)
	{
	case ERROR_FILE_NOT_FOUND:
		return std::string("The requested file couldn't be found.");
		break;
	case ERROR_FILE_EXISTS:
		return std::string("The requested file already exists.");
		break;
	default:
		return std::string("Unknown Win32 error code ") + std::to_string(errorCode) + std::string(".");
		break;
	}
}

std::string CreateWin32ErrorMessage(DWORD errorCode)
{
	std::string error = std::string("Failed to open file. Reason: ") + TranslateWin32ErrorCode(errorCode);
	return error;
}

FileNavigator_Win32::FileNavigator_Win32()
{

}

FileNavigator_Win32::~FileNavigator_Win32()
{

}

Expected<std::shared_ptr<File>, std::string> FileNavigator_Win32::CreateFile(std::string filepath, FileAccessMode accessMode, bool overwriteExisting)
{
	std::string directory = IsolateDirectory(filepath);
	if (directory.size() != 0 && !FindDirectory(directory))
	{
		return Expected<std::shared_ptr<File>, std::string>(UnexpectedWrapper<std::string>("Failed to create file. Reason: The directory specified doesn't exist."));
	}

	DWORD desiredAccess = GetWin32AccessMode(accessMode);
	DWORD shareMode = GetWin32ShareMode(accessMode);
	DWORD creationDisposition = (overwriteExisting) ? CREATE_ALWAYS : CREATE_NEW;
	
	HANDLE fileHandle = CreateFileA(filepath.c_str(), desiredAccess, shareMode, NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD errorCode = GetLastError();
		return Expected<std::shared_ptr<File>, std::string>(UnexpectedWrapper<std::string>(CreateWin32ErrorMessage(errorCode)));
	}

	return std::make_shared<File_Win32>(fileHandle, accessMode);
}

Expected<std::shared_ptr<File>, std::string> FileNavigator_Win32::OpenFile(std::string filepath, FileAccessMode accessMode, bool openAlways)
{
	DWORD desiredAccess = GetWin32AccessMode(accessMode);
	DWORD shareMode = GetWin32ShareMode(accessMode);
	DWORD creationDisposition = (openAlways) ? OPEN_ALWAYS : OPEN_EXISTING;

	if (creationDisposition == OPEN_ALWAYS)
	{
		std::string directory = IsolateDirectory(filepath);
		if (directory.size() != 0 && !FindDirectory(directory))
		{
			return Expected<std::shared_ptr<File>, std::string>(UnexpectedWrapper<std::string>("Failed to create file on opening. Reason: The directory specified doesn't exist."));
		}
	}

	HANDLE fileHandle = CreateFileA(filepath.c_str(), desiredAccess, shareMode, NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD errorCode = GetLastError();
		return Expected<std::shared_ptr<File>, std::string>(UnexpectedWrapper<std::string>(CreateWin32ErrorMessage(errorCode)));
	}

	return std::make_shared<File_Win32>(fileHandle, accessMode);
}

bool FileNavigator_Win32::DeleteFile(std::string filepath)
{
	return false;
}

bool FileNavigator_Win32::FindFile(std::string filepath)
{
	if (filepath.size() == 0)
	{
		return false;
	}

	HANDLE findHandle;
	WIN32_FIND_DATAA findData;
	findHandle = FindFirstFileA(filepath.c_str(), &findData);

	bool isFile = !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	bool isValid = findHandle != INVALID_HANDLE_VALUE;
	bool result = isFile && isValid;
	FindClose(findHandle);

	return result;
}

bool FileNavigator_Win32::FindDirectory(std::string directory)
{
	if (directory.size() == 0)
	{
		return false;
	}

	HANDLE findHandle;
	WIN32_FIND_DATAA findData;
	findHandle = FindFirstFileA(directory.c_str(), &findData);

	bool isDirectory = findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	bool isValid = findHandle != INVALID_HANDLE_VALUE;
	bool result = isDirectory && isValid;
	FindClose(findHandle);

	return result;
}

}