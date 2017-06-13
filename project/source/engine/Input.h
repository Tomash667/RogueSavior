#pragma once

#include "Key.h"

class InputState
{
	enum State
	{
		IS_UP,			// 00
		IS_RELEASED,	// 01
		IS_DOWN,		// 10
		IS_PRESSED		// 11
	};

public:
	// Return true if key was pressed
	bool Pressed(byte key) const
	{
		return keystate[key] == IS_PRESSED;
	}
	// Return true if key was released
	bool Released(byte key) const
	{
		return keystate[key] == IS_RELEASED;
	}
	// Return true if key is down
	bool Down(byte key) const
	{
		return keystate[key] >= IS_DOWN;
	}
	// Return true if key is up
	bool Up(byte key) const
	{
		return keystate[key] <= IS_RELEASED;
	}
	// Return true if key was pressed and set new state
	bool PressedRelease(byte key, State new_state = IS_DOWN)
	{
		if(Pressed(key))
		{
			keystate[key] = new_state;
			return true;
		}
		else
			return false;
	}
	// Shortcut, checks if other modifiers are not down
	// for example: Ctrl+A, shift and alt must not be pressed
	bool Shortcut(int modifier, byte key, bool up = true)
	{
		if(shortcut_state == modifier && Pressed(key))
		{
			if(up)
				keystate[key] = IS_DOWN;
			return true;
		}
		else
			return false;
	}
	// Return true if pressed or should repeat operation
	bool DownRepeat(byte key)
	{
		return Down(key) && keyrepeat[key];
	}
	// Return true if double clicked mouse button
	bool DoubleClick(byte key)
	{
		assert(key >= K_LBUTTON && key <= K_XBUTTON2);
		return doubleclk[key];
	}
	const INT2& GetMouseMove()
	{
		return mouse_move;
	}

	void Update();
	void UpdateShortcuts();
	void UpdateMouse(const INT2& move)
	{
		mouse_move = move;
	}
	void Process(byte key, bool down);
	void ProcessDoubleClick(byte key);
	void ReleaseKeys();

private:
	byte keystate[256];
	bool keyrepeat[256];
	bool doubleclk[6];
	int shortcut_state;
	INT2 mouse_move;
};

extern InputState Input;
