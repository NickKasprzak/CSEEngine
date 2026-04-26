#include "AppPlatform_SDL3.h"
#include "../input/Keyboard.h"

namespace CSEApplication
{

KeyType SDLKeycodeToKeyType(SDL_Keycode keycode);

bool IsKeyboardEvent(SDL_Event& event)
{
	uint32_t eventType = event.type;

	if (eventType == SDL_EVENT_KEY_DOWN || eventType == SDL_EVENT_KEY_UP)
	{
		return true;
	}

	return false;
}

KeyboardInputEvent CreateKeyboardEvent(uint32_t eventType, SDL_KeyboardEvent& keyEvent)
{
	KeyboardInputEvent event;
	event.keyType = SDLKeycodeToKeyType(keyEvent.key);
	
	if (!keyEvent.down)
	{
		event.keyPressState = KEY_UP;
	}
	else if (keyEvent.down && !keyEvent.repeat)
	{
		event.keyPressState = KEY_DOWN;
	}
	else if (keyEvent.down && keyEvent.repeat)
	{
		event.keyPressState = KEY_HELD;
	}

	return event;
}

bool IsWindowEvent(SDL_Event& event)
{
	uint32_t eventType = event.type;

	if (eventType >= SDL_EVENT_WINDOW_FIRST && eventType <= SDL_EVENT_WINDOW_LAST)
	{
		return true;
	}

	return false;
}

WindowEvent CreateWindowEvent(uint32_t eventType, SDL_WindowEvent& windowEvent)
{
	WindowEvent event;
	event.type = WINDOW_EVENT_INVALID;

	if (eventType == SDL_EVENT_WINDOW_RESIZED)
	{
		event.type = WINDOW_EVENT_RESIZED;
		event.eventUnion.resizeEvent.newHeight = windowEvent.data1;
		event.eventUnion.resizeEvent.newHeight = windowEvent.data2;
	}

	return event;
}

AppPlatform_SDL3::AppPlatform_SDL3()
	: _windowEventDistributor(), _inputEventQueue(), _quitCallback()
{

}

AppPlatform_SDL3::~AppPlatform_SDL3()
{

}

void AppPlatform_SDL3::Startup(std::string name, uint16_t winWidth, uint16_t winHeight)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	_window = Window_SDL3(name, winWidth, winHeight);
}

void AppPlatform_SDL3::Shutdown()
{
	SDL_Quit();
}

void AppPlatform_SDL3::PollPlatformEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (IsWindowEvent(event))
		{
			WindowEvent windowEvent = CreateWindowEvent(event.type, event.window);
			windowEvent.window = &_window;
			_windowEventDistributor.PushEvent(windowEvent.type, windowEvent);
		}

		else if (IsKeyboardEvent(event))
		{
			KeyboardInputEvent keyboardEvent = CreateKeyboardEvent(event.type, event.key);

			InputEvent inputEvent;
			inputEvent.type = INPUT_TYPE_KEYBOARD;
			inputEvent.eventUnion.keyboard = keyboardEvent;

			_inputEventQueue.PushEvent(INPUT_TYPE_KEYBOARD, inputEvent);
		}

		else if (event.type == SDL_EVENT_QUIT)
		{
			_quitCallback.Invoke();
		}
	}
}

Window* AppPlatform_SDL3::GetWindow()
{
	return &_window;
}

void AppPlatform_SDL3::AddWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue)
{
	_windowEventDistributor.AddEventQueue(eventType, queue);
}

void AppPlatform_SDL3::RemoveWindowEventListener(WindowEventType eventType, CSECore::EventQueue<WindowEvent>* queue)
{
	_windowEventDistributor.RemoveEventQueue(eventType, queue);
}

void AppPlatform_SDL3::RegisterInputQueueToInputManager(InputManager& inputManager)
{
	inputManager.SetPlatformInputEventQueue(&_inputEventQueue);
}

void AppPlatform_SDL3::SetQuitCallback(CSECore::EventCallback<> callback)
{
	_quitCallback = callback;
}

KeyType SDLKeycodeToKeyType(SDL_Keycode keycode)
{
	switch (keycode)
	{
	case SDLK_A:
		return KEY_A;
	case SDLK_B:
		return KEY_B;
	case SDLK_C:
		return KEY_C;
	case SDLK_D:
		return KEY_D;
	case SDLK_E:
		return KEY_E;
	case SDLK_F:
		return KEY_F;
	case SDLK_G:
		return KEY_G;
	case SDLK_H:
		return KEY_H;
	case SDLK_I:
		return KEY_I;
	case SDLK_J:
		return KEY_J;
	case SDLK_K:
		return KEY_K;
	case SDLK_L:
		return KEY_L;
	case SDLK_M:
		return KEY_M;
	case SDLK_N:
		return KEY_N;
	case SDLK_O:
		return KEY_O;
	case SDLK_P:
		return KEY_P;
	case SDLK_Q:
		return KEY_Q;
	case SDLK_R:
		return KEY_R;
	case SDLK_S:
		return KEY_S;
	case SDLK_T:
		return KEY_T;
	case SDLK_U:
		return KEY_U;
	case SDLK_V:
		return KEY_V;
	case SDLK_W:
		return KEY_W;
	case SDLK_X:
		return KEY_X;
	case SDLK_Y:
		return KEY_Y;
	case SDLK_Z:
		return KEY_Z;

	case SDLK_SPACE:
		return KEY_SPACE;
	case SDLK_ESCAPE:
		return KEY_ESCAPE;
	case SDLK_RETURN:
		return KEY_ENTER;
	case SDLK_LSHIFT:
		return KEY_LSHIFT;
	case SDLK_RSHIFT:
		return KEY_RSHIFT;
	case SDLK_BACKSPACE:
		return KEY_BACKSPACE;

	default:
		return KEY_COUNT;
	}
}

}