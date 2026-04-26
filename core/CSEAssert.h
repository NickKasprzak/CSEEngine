#pragma once
#include <sstream>

namespace CSECore
{

void AbortProgram(std::stringstream reason);

#if ASSERTS_ENABLED
#define CSE_ASSERT(condition, message)\
{\
	if (!(condition))\
	{\
		CSECore::AbortProgram(std::stringstream() << "Assertion failed in file " << __FILE__ << " on line " << __LINE__ << ". Reason: " << message);\
	}\
}
#else
#define CSE_ASSERT(condition, message) {}
#endif

}