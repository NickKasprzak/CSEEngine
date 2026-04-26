#pragma once
#include "Window.h"
#include "SDL3/SDL.h"
#include <string>

namespace CSEApplication
{

class Window_SDL3 : public Window
{
public:
	Window_SDL3();
	Window_SDL3(std::string windowName, uint16_t width, uint16_t height);
	~Window_SDL3();

	virtual void ResizeWindow(uint16_t newWidth, uint16_t newHeight) override;
	virtual void SetWindowMode(WindowMode mode) override;
	virtual CSECore::Any<64> GetPlatformWindowInfo() const override;

private:
	SDL_Window* _window;
};

typedef Window_SDL3 Window_Impl;

}