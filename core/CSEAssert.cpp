#include "CSEAssert.h"
#include "Logger.h"

namespace CSECore
{

void AbortProgram(std::stringstream reason)
{
	CSE_LOGE(reason.str());
	std::abort();
}

}