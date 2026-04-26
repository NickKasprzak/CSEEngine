#include "Window_SDL3.h"
#include "PlatformWindowInfo_App.h"

namespace CSEApplication
{

CSECore::Any<64> CreatePlatformWindowInfo(SDL_Window* window);

Window_SDL3::Window_SDL3()
	: Window(), _window(nullptr)
{

}

Window_SDL3::Window_SDL3(std::string windowName, uint16_t width, uint16_t height)
	: Window(), _window(nullptr)
{
	_window = SDL_CreateWindow(windowName.c_str(), width, height, 0);
}

Window_SDL3::~Window_SDL3()
{

}

void Window_SDL3::ResizeWindow(uint16_t newWidth, uint16_t newHeight)
{
	SDL_SetWindowSize(_window, newWidth, newHeight);
}

void Window_SDL3::SetWindowMode(WindowMode mode)
{
	if (mode == WINDOW_MODE_WINDOWED)
	{
		SDL_SetWindowFullscreen(_window, false);
	}
	else if (mode == WINDOW_MODE_FULLSCREEN)
	{
		SDL_SetWindowFullscreen(_window, true);
	}
}

CSECore::Any<64> Window_SDL3::GetPlatformWindowInfo() const
{
	return CreatePlatformWindowInfo(_window);
}

#if WIN32

CSECore::Any<64> CreatePlatformWindowInfo(SDL_Window* window)
{
	SDL_PropertiesID propID = SDL_GetWindowProperties(window);
	HINSTANCE hinst = reinterpret_cast<HINSTANCE>(SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr));
	HWND hwnd = reinterpret_cast<HWND>(SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
	
	int width = 0;
	int height = 0;
	SDL_GetWindowSize(window, &width, &height);

	PlatformWindowInfo_Win32 windowInfo;
	windowInfo.hinst = hinst;
	windowInfo.hwnd = hwnd;
	windowInfo.width = width;
	windowInfo.height = height;

	return CSECore::Any<64>(windowInfo);
}

#endif

}