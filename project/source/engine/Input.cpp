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
	}
	if(printscreen == IS_PRESSED)
		keystate[K_PRINTSCREEN] = IS_RELEASED;
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

void InputState::ReleaseKeys()
{
	for(uint i = 0; i < 255; ++i)
	{
		if(keystate[i] & 0x2)
			keystate[i] = IS_RELEASED;
	}
}
