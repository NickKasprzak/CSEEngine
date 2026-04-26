#pragma once
#include "eventsystem/EventDistributor.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace CSEApplication
{

enum InputType
{
	INPUT_TYPE_KEYBOARD,
	INPUT_TYPE_MOUSE
};

struct InputEvent
{
	InputType type;
	union EventUnion
	{
		KeyboardInputEvent keyboard;
	} eventUnion;
};

class InputManager
{
	friend class App;
public:
	~InputManager();

	/*
	* Used to set the event queue that
	* recieves input events from the 
	* platform layer
	*/
	void SetPlatformInputEventQueue(CSECore::EventQueue<InputEvent>* inputEventQueue);
	void Update();

	static const Keyboard& GetKeyboard();
	// get const mouse ref

	static void AddInputEventListener(InputType inputType, CSECore::EventQueue<InputEvent>* queue);
	static void RemoveInputEventListener(InputType inputType, CSECore::EventQueue<InputEvent>* queue);

	static void AddKeyboardEventListener(KeyType keyType, CSECore::EventQueue<KeyboardInputEvent>* queue);
	static void RemoveKeyboardEventListener(KeyType keyType, CSECore::EventQueue<KeyboardInputEvent>* queue);

	/*
	* Mouse events might need special
	* handling for specific types of
	* mouse input like mouse buttons
	* and mouse movement
	*/

private:
	static Keyboard _keyboard;
	static CSECore::EventQueue<InputEvent>* _inputEventQueue;
	static CSECore::EventDistributor<InputEvent> _inputEventDistributor;
	static CSECore::EventDistributor<KeyboardInputEvent> _keyboardEventDistributor;

	InputManager();

	void _processPlatformInputEvents();
	void _handleKeyboardInputEvent(InputEvent event);
	void _distributeKeyboardInputEvents();
};

}