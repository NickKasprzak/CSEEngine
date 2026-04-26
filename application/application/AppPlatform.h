#pragma once
#include "../window/WindowEvent.h"
#include "../input/InputManager.h"
#include "eventsystem/EventQueue.h"
#include "eventsystem/EventDistributor.h"

namespace CSEApplication
{

class AppPlatform
{
public:
	virtual void Startup(std::string name, uint16_t winWidth, uint16_t winHeight) = 0;
	virtual void Shutdown() = 0;

	virtual void PollPlatformEvents() = 0;

	virtual Window* GetWindow() = 0;
	virtual void AddWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue) = 0;
	virtual void RemoveWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>*queue) = 0;

	virtual void RegisterInputQueueToInputManager(InputManager& inputManager) = 0;
	virtual void SetQuitCallback(CSECore::EventCallback<> callback) = 0;
};

}