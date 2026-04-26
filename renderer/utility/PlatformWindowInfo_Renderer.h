#pragma once
#include <cstdint>

#if WIN32
#include "Windows.h"
#endif

namespace CSERenderer
{

#if WIN32

struct PlatformWindowInfo_Win32
{
	HWND hwnd;
	HINSTANCE hinst;
	uint16_t width;
	uint16_t height;
};

#endif

}