#pragma once
#include <cstdint>

namespace CSEApplication
{

class Window;

enum WindowEventType
{
	WINDOW_EVENT_RESIZED,
	WINDOW_EVENT_INVALID
};

struct WindowResizeEvent
{
	uint16_t newWidth;
	uint16_t newHeight;
};

struct WindowEvent
{
	const Window* window;
	WindowEventType type;
	union EventUnion
	{
		WindowResizeEvent resizeEvent;
	} eventUnion;
};

}