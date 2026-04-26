#include "Logger.h"
#include "CSEAssert.h"
#include "Expected.h"

CSECore::Expected<int, std::string> testfunc(int value)
{
	if (value < 10)
	{
		return CSECore::CreateUnexpected<int, std::string>("This string is too small.");
	}

	return CSECore::CreateExpected<int, std::string>(value * 100);
}

int main()
{
	CSECore::Expected<int, std::string> valresult1 = testfunc(20);
	CSE_ASSERT(valresult1.HasExpected(), "Did not have expected when it was supposed to.");
	CSE_ASSERT(!valresult1.HasUnexpected(), "Had unexpected when it wasn't supposed to.");
	CSE_LOGI("valresult1 ExpectedValue test result: " << valresult1.GetExpected());

	CSECore::Expected<int, std::string> valresult2 = testfunc(5);
	CSE_ASSERT(valresult2.HasUnexpected(), "Did not have unexpected when it was supposed to.");
	CSE_ASSERT(!valresult2.HasExpected(), "Had expected when it wasn't supposed to.");
	CSE_LOGI("valresult2 UnexpectedValue test result: " << valresult2.GetUnexpected());

	valresult1 = valresult2;
	CSE_ASSERT(valresult1.HasUnexpected(), "Did not have unexpected after copying like it was supposed to.");
	CSE_ASSERT(!valresult1.HasExpected(), "Had expected after copying like it wasn't supposed to.");
	CSE_LOGI("valresult1 copy from valresult2 UnexpectedValue test result: " << valresult1.GetUnexpected());
}