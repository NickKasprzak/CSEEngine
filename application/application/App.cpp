#include "App.h"
#include "AppPlatform_Impl.h"
#include "../window/Window_Impl.h"

namespace CSEApplication
{

App::App()
	: _shouldQuit(false), _appName(), _appPlatform(nullptr), _inputManager()
{

}

App::~App()
{

}

void App::Initialize(std::string name)
{
	_appName = name;
	_appPlatform = new AppPlatform_Impl();

	_appPlatform->Startup(name, 640, 480);

	CSECore::EventCallback<> quitCallback;
	quitCallback.BindCallback<App, &App::RequestQuit>(this);
	_appPlatform->SetQuitCallback(quitCallback);

	_appPlatform->RegisterInputQueueToInputManager(_inputManager);
}

void App::Dispose()
{
	_appPlatform->Shutdown();
	delete _appPlatform;
}

const Window* App::GetWindow()
{
	return _appPlatform->GetWindow();
}

void App::PollPlatformEvents()
{
	_appPlatform->PollPlatformEvents();
}

void App::AddWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue)
{
	_appPlatform->AddWindowEventListener(eventType, queue);
}

void App::RemoveWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue)
{
	_appPlatform->RemoveWindowEventListener(eventType, queue);
}

bool App::ShouldQuit()
{
	return _shouldQuit;
}

void App::RequestQuit()
{
	_shouldQuit = true;
}

void App::UpdateInputManager()
{
	_inputManager.Update();
}

}