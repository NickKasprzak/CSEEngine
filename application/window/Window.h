#pragma once
#include "Any.h"
#include <cstdint>

namespace CSEApplication
{

enum WindowMode
{
	WINDOW_MODE_WINDOWED,
	WINDOW_MODE_FULLSCREEN
};

class Window
{
public:
	virtual ~Window() {}

	virtual void ResizeWindow(uint16_t newWidth, uint16_t newHeight) = 0;
	virtual void SetWindowMode(WindowMode mode) = 0;
	virtual CSECore::Any<64> GetPlatformWindowInfo() const = 0;
};

}