#include "Pch.h"
#include "Core.h"
#include "Input.h"

InputState Input;

// Change keys state from pressed->down and released->up
// Special printscreen handling
void InputState::Update()
{
	byte printscreen = keystate[K_PRINTSCREEN];
	for(uint i = 0; i < 256; ++i)
	{
		if(keystate[i] & 1)
			--keystate[i];
		keyrepeat[i] = false;
	}
	for(uint i = 0; i < 6; ++i)
		doubleclk[i] = false;
	if(printscreen == IS_PRESSED)
		keystate[K_PRINTSCREEN] = IS_RELEASED;
	mouse_move = INT2(0, 0);
	mouse_wheel = 0;
}

// Update shortcuts state
void InputState::UpdateShortcuts()
{
	shortcut_state = 0;
	if(Down(K_SHIFT))
		shortcut_state |= KEY_SHIFT;
	if(Down(K_CONTROL))
		shortcut_state |= KEY_CONTROL;
	if(Down(K_ALT))
		shortcut_state |= KEY_ALT;
}

// Handle key down/up
void InputState::Process(byte key, bool down)
{
	if(key != K_PRINTSCREEN)
	{
		if(down)
		{
			if(keystate[key] <= IS_RELEASED)
				keystate[key] = IS_PRESSED;
		}
		else
		{
			if(keystate[key] >= IS_DOWN)
				keystate[key] = IS_RELEASED;
		}
	}
	else
		keystate[key] = IS_PRESSED;
}

// Process double click
void InputState::ProcessDoubleClick(byte key)
{
	assert(key >= K_LBUTTON && key <= K_XBUTTON2);
	Process(key, true);
	doubleclk[key] = true;
}

// Release all keys
void InputState::ReleaseKeys()
{
	for(uint i = 0; i < 255; ++i)
	{
		if(keystate[i] & 0x2)
			keystate[i] = IS_RELEASED;
	}
}
