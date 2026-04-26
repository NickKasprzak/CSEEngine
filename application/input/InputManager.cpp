#include "InputManager.h"

namespace CSEApplication
{

Keyboard InputManager::_keyboard;
CSECore::EventQueue<InputEvent>* InputManager::_inputEventQueue = nullptr;
CSECore::EventDistributor<InputEvent> InputManager::_inputEventDistributor;
CSECore::EventDistributor<KeyboardInputEvent> InputManager::_keyboardEventDistributor;

InputManager::InputManager()
{

}

InputManager::~InputManager()
{

}

void InputManager::SetPlatformInputEventQueue(CSECore::EventQueue<InputEvent>* inputEventQueue)
{
	_inputEventQueue = inputEventQueue;

	CSECore::EventCallback<InputEvent> keyboardCallback;
	keyboardCallback.BindCallback<InputManager, &InputManager::_handleKeyboardInputEvent>(this);

	CSECore::EventCallbackList<InputEvent> callbackList;
	callbackList.AddCallback(INPUT_TYPE_KEYBOARD, keyboardCallback);

	_inputEventQueue->BindCallbackList(callbackList);
}

void InputManager::Update()
{
	_keyboard.FlipKeyBuffers();

	_processPlatformInputEvents();
	_distributeKeyboardInputEvents();
}

const Keyboard& InputManager::GetKeyboard()
{
	return _keyboard;
}

void InputManager::AddInputEventListener(InputType inputType, CSECore::EventQueue<InputEvent>* queue)
{
	_inputEventDistributor.AddEventQueue(inputType, queue);
}

void InputManager::RemoveInputEventListener(InputType inputType, CSECore::EventQueue<InputEvent>* queue)
{
	_inputEventDistributor.RemoveEventQueue(inputType, queue);
}

void InputManager::AddKeyboardEventListener(KeyType keyType, CSECore::EventQueue<KeyboardInputEvent>* queue)
{
	_keyboardEventDistributor.AddEventQueue(keyType, queue);
}

void InputManager::RemoveKeyboardEventListener(KeyType keyType, CSECore::EventQueue<KeyboardInputEvent>* queue)
{
	_keyboardEventDistributor.RemoveEventQueue(keyType, queue);
}

void InputManager::_processPlatformInputEvents()
{
	_inputEventQueue->Process();
}

void InputManager::_handleKeyboardInputEvent(InputEvent event)
{
	KeyboardInputEvent keyboardEvent = event.eventUnion.keyboard;
	_keyboard.SetKeyPressState(keyboardEvent.keyType, keyboardEvent.keyPressState);
}

void InputManager::_distributeKeyboardInputEvents()
{
	for (int i = 0; i < KEY_COUNT; i++)
	{
		KeyType type = (KeyType)i;
		KeyPressState state = _keyboard.GetKeyPressState(type);

		if (type == KEY_H)
		{
			if (state == KEY_DOWN) { CSE_LOGI("down."); }
			else if (state == KEY_HELD) { CSE_LOGI("held."); }
			else if (state == KEY_UP) { CSE_LOGI("up."); }
			//else { CSE_LOGI("none."); }
		}

		if (state != KEY_NONE)
		{
			KeyboardInputEvent keyboardEvent;
			keyboardEvent.keyType = type;
			keyboardEvent.keyPressState = state;
			_keyboardEventDistributor.PushEvent(type, keyboardEvent);

			InputEvent inputEvent;
			inputEvent.type = INPUT_TYPE_KEYBOARD;
			inputEvent.eventUnion.keyboard = keyboardEvent;
			_inputEventDistributor.PushEvent(INPUT_TYPE_KEYBOARD, inputEvent);
		}
	}
}

}