#pragma once

namespace CSECore
{

class File
{
public:
	virtual ~File() {};

	virtual size_t Read(void* buffer, size_t bufferSize, size_t amount) = 0;
	virtual size_t Write(void* buffer, size_t bufferSize, size_t amount) = 0;

	virtual size_t Seek(size_t position) = 0;
	virtual size_t SeekBegin() = 0;
	virtual size_t SeekEnd() = 0;

	virtual size_t Tell() = 0;

	virtual bool Flush() = 0;
};

}