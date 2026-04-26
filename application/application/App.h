#pragma once
#include "AppPlatform.h"
#include "../window/Window.h"
#include "../window/WindowEvent.h"
#include "../input/InputManager.h"
#include "eventsystem/EventQueue.h"
#include <string>

namespace CSEApplication
{

class App
{
public:
	App();
	~App();

	const Window* GetWindow();

	void PollPlatformEvents();
	void AddWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue);
	void RemoveWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue);

protected:
	virtual void Initialize(std::string name);
	virtual void Run() = 0;
	virtual void Dispose();

	bool ShouldQuit();
	void RequestQuit();
	void UpdateInputManager();

private:
	bool _shouldQuit;
	std::string _appName;
	AppPlatform* _appPlatform;
	InputManager _inputManager;
};

}