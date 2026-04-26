#pragma once
#include "AppPlatform.h"
#include "../window/Window_SDL3.h"

namespace CSEApplication
{

class AppPlatform_SDL3 : public AppPlatform
{
public:
	AppPlatform_SDL3();
	~AppPlatform_SDL3();

	virtual void Startup(std::string name, uint16_t winWidth, uint16_t winHeight) override;
	virtual void Shutdown() override;

	virtual void PollPlatformEvents() override;

	virtual Window* GetWindow() override;
	virtual void AddWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue) override;
	virtual void RemoveWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue) override;

	virtual void RegisterInputQueueToInputManager(InputManager& inputManager) override;
	virtual void SetQuitCallback(CSECore::EventCallback<> callback) override;

private:
	Window_SDL3 _window;
	CSECore::EventDistributor<WindowEvent> _windowEventDistributor;
	CSECore::EventQueue<InputEvent> _inputEventQueue;
	CSECore::EventCallback<> _quitCallback;
	
};

typedef AppPlatform_SDL3 AppPlatform_Impl;

}