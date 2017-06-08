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
	bool Pressed(byte key) const { return keystate[key] == IS_PRESSED; }
	bool Released(byte key) const { return keystate[key] == IS_RELEASED; }
	bool Down(byte key) const { return keystate[key] >= IS_DOWN; }
	bool Up(byte key) const { return keystate[key] <= IS_RELEASED; }
	void Update();
	void Process(byte key, bool down);
	void ReleaseKeys();

private:
	byte keystate[256];
};

extern InputState Input;
