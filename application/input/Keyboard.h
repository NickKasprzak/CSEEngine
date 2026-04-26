#pragma once
#include <cstdint>
#include <set>
#include <array>

namespace CSEApplication
{

enum KeyType
{
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_SPACE,
	KEY_ESCAPE,
	KEY_ENTER,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_BACKSPACE,

	KEY_COUNT
};

enum KeyPressState
{
	KEY_NONE = 0,
	KEY_UP,
	KEY_DOWN,
	KEY_HELD
};

struct KeyboardInputEvent
{
	KeyType keyType;
	KeyPressState keyPressState;
};

class Keyboard
{
public:
	Keyboard()
		: _keyStatesCurrent(), _keyStatesPrevious()
	{

	}

	~Keyboard()
	{

	}

	/*
	* Returns the requested key's state.
	*/
	KeyPressState GetKeyPressState(KeyType type)
	{
		return _keyStatesCurrent[type];
	}

	void SetKeyPressState(KeyType type, KeyPressState state)
	{
		if (type >= KEY_COUNT || type < 0)
		{
			return;
		}

		_keyStatesCurrent[type] = state;
	}

	/*
	* Flip key buffers, effectively
	* shifting all keyboard inputs
	* to represent the previous frame
	* rather than the current frame
	*/
	void FlipKeyBuffers()
	{
		_keyStatesPrevious = _keyStatesCurrent;

		/* 
		* Assume that any DOWN key in previous will
		* transition to HELD. It will be assigned
		* UP via SetKeyPressState if it should be 
		* otherwise.
		* 
		* Otherwise, set it to NONE.
		*/
		for (int i = 0; i < KEY_COUNT; i++)
		{
			if (_keyStatesPrevious[i] == KEY_DOWN || _keyStatesPrevious[i] == KEY_HELD)
			{
				_keyStatesCurrent[i] = KEY_HELD;
				continue;
			}

			_keyStatesCurrent[i] = KEY_NONE;
		}
	}

private:

	/*
	* Each key has state.
	*
	* NONE will be the default state and
	* the one that each key will be in when
	* they were in the UP state on the
	* previous frame
	*
	* DOWN will be the state a key will be
	* in upon initially being pressed
	*
	* HELD will be the state a key will
	* transition to if they were in the
	* DOWN state the prior frame wasn't
	* set to the UP state on the current
	* frame.
	*
	* UP will be the state a key will be in
	* upon being released after being pressed.
	*/

	std::array<KeyPressState, KeyType::KEY_COUNT> _keyStatesCurrent;
	std::array<KeyPressState, KeyType::KEY_COUNT> _keyStatesPrevious;
};

inline std::string KeyTypeToString(KeyType type)
{
	switch (type)
	{
		case KEY_A:
			return "A";
		case KEY_B:
			return "B";
		case KEY_C:
			return "C";
		case KEY_D:
			return "D";
		case KEY_E:
			return "E";
		case KEY_F:
			return "F";
		case KEY_G:
			return "G";
		case KEY_H:
			return "H";
		case KEY_I:
			return "I";
		case KEY_J:
			return "J";
		case KEY_K:
			return "K";
		case KEY_L:
			return "L";
		case KEY_M:
			return "M";
		case KEY_N:
			return "N";
		case KEY_O:
			return "O";
		case KEY_P:
			return "P";
		case KEY_Q:
			return "Q";
		case KEY_R:
			return "R";
		case KEY_S:
			return "S";
		case KEY_T:
			return "T";
		case KEY_U:
			return "U";
		case KEY_V:
			return "V";
		case KEY_W:
			return "W";
		case KEY_X:
			return "X";
		case KEY_Y:
			return "Y";
		case KEY_Z:
			return "Z";

		case KEY_SPACE:
			return "Space";
		case KEY_ESCAPE:
			return "Escape";
		case KEY_ENTER:
			return "Enter";
		case KEY_LSHIFT:
			return "Left Shift";
		case KEY_RSHIFT:
			return "Right Shift";
		case KEY_BACKSPACE:
			return "Backspace";

		default:
			return " ";
	}
}

}