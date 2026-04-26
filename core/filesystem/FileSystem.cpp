#include "FileSystem.h"

namespace CSECore
{

FileSystem::FileSystem()
	: _navigator(nullptr)
{

}

FileSystem::~FileSystem()
{

}

Expected<std::shared_ptr<File>, std::string> FileSystem::CreateFile(std::string filepath, FileAccessMode accessMode, bool overwriteExisting)
{
	return _navigator->CreateFile(filepath, accessMode, overwriteExisting);
}

Expected<std::shared_ptr<File>, std::string> FileSystem::OpenFile(std::string filepath, FileAccessMode accessMode, bool openAlways)
{
	return _navigator->OpenFile(filepath, accessMode, openAlways);
}

bool FileSystem::DeleteFile(std::string filepath)
{
	return _navigator->DeleteFile(filepath);
}

bool FileSystem::FindFile(std::string filepath)
{
	return _navigator->FindFile(filepath);
}


}