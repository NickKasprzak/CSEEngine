#pragma once
#include "filesystem/FileSystem.h"
#include "filesystem/navigators/FileNavigator_Device.h"
#include "Logger.h"
#include <array>

int main()
{
	CSECore::FileSystem fileSystem;
	fileSystem.SetNavigator<CSECore::FileNavigator_Device>();
	
	// Test for open failure
	{
		CSECore::Expected<std::shared_ptr<CSECore::File>, std::string> openFake = fileSystem.OpenFile("notreal/pleasedont.txt", CSECore::FileAccessMode::ACCESS_READ_WRITE, false);
		CSE_ASSERT(!openFake.HasExpected(), "An attempt to open a fake file succeeded when it shouldn't have.");
	}

	// Test for read only access
	{
		CSECore::Expected<std::shared_ptr<CSECore::File>, std::string> openReal = fileSystem.OpenFile("testdir/read.txt", CSECore::FileAccessMode::ACCESS_READ, false);
		CSE_ASSERT(openReal.HasExpected(), "An attempt to open a real file didn't succeed when it should've.");

		std::shared_ptr<CSECore::File> readFile = openReal.GetExpected();

		std::array<char, 256> readBuffer;
		size_t amountRead1 = readFile->Read(readBuffer.data(), readBuffer.size(), 18);
		std::string readString(readBuffer.data(), amountRead1);
		CSE_ASSERT(readString == "wowie i have text!", "File read didn't produce the expected outcome. Read " << amountRead1 << " chars and got a result of: " << readString);

		readBuffer.fill(0);
#ifdef WIN32
		readFile->Seek(30 + 12); // Newlines on windows are two chars (/n/r), hence the addtional 12
#else
		readFile->Seek(30);
#endif
		size_t amountRead2 = readFile->Read(readBuffer.data(), readBuffer.size(), readBuffer.size());
		std::string readString2(readBuffer.data(), amountRead2);
		CSE_ASSERT(readString2 == "woah this is at an offset!", "File read didn't produce the expected outcome. Read " << amountRead2 << " chars and got a result of: " << readString2);

		//readFile->Write("This write should fail!", sizeof("This write should fail!"), sizeof("This write should fail!"));
	}

	// Test for write only access
	{
		CSECore::Expected<std::shared_ptr<CSECore::File>, std::string> openReal = fileSystem.OpenFile("testdir/write.txt", CSECore::FileAccessMode::ACCESS_WRITE, false);
		CSE_ASSERT(openReal.HasExpected(), "An attempt to open a real file didn't succeed when it should've.");

		std::shared_ptr<CSECore::File> writeFile = openReal.GetExpected();

		size_t amountWritten1 = writeFile->Write("From the top!", sizeof("From the top!"), sizeof("From the top!"));
		CSE_ASSERT(amountWritten1 == sizeof("From the top!"), "File write didn't write the expected number of characters. Amount written was " << amountWritten1);

		writeFile->Seek(sizeof("From the top!") + 5);

		size_t amountWritten2 = writeFile->Write("From 5 characters later!", sizeof("From 5 characters later!"), sizeof("From 5 characters later!"));
		CSE_ASSERT(amountWritten2 == sizeof("From 5 characters later!"), "File write didn't write the expected number of characters. Amount written was " << amountWritten2);

		//writeFile->Read(nullptr, 0, 0); // This should also fail!
	}

	// Test for read and write access
	{
		CSECore::Expected<std::shared_ptr<CSECore::File>, std::string> openReal = fileSystem.OpenFile("testdir/readwrite.txt", CSECore::FileAccessMode::ACCESS_READ_WRITE, false);
		CSE_ASSERT(openReal.HasExpected(), "An attempt to open a real file didn't succeed when it should've.");

		std::shared_ptr<CSECore::File> file = openReal.GetExpected();

		size_t amountWritten = file->Write("I can read & write!", sizeof("I can read & write!"), sizeof("I can read & write!"));
		CSE_ASSERT(amountWritten == sizeof("I can read & write!"), "File write didn't write the expected number of characters. Amount written was " << amountWritten);

		file->SeekBegin();

		std::array<char, 256> readBuffer;
		size_t amountRead = file->Read(readBuffer.data(), readBuffer.size(), readBuffer.size()); 
		std::string readString(readBuffer.data(), amountRead - 1); // The /0 written above gets read in here, hence why its one less char.
		CSE_ASSERT(readString == "I can read & write!", "File read didn't produce the expected outcome. Read " << amountRead << " chars and got a result of: " << readString);
	}

	CSE_LOGI("Success.");
}