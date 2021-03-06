#pragma once

enum Key
{
	K_NONE = 0x00,
	K_LBUTTON = 0x01,
	K_RBUTTON = 0x02,
	K_MBUTTON = 0x04,
	K_XBUTTON1 = 0x05,
	K_XBUTTON2 = 0x06,
	K_BACKSPACE = 0x08,
	K_TAB = 0x09,
	K_ENTER = 0x0D,
	K_SHIFT = 0x10,
	K_CONTROL = 0x11,
	K_ALT = 0x12,
	K_PAUSE = 0x13,
	K_CAPS_LOCK = 0x14,
	K_ESCAPE = 0x1B,
	K_SPACE = 0x20,
	K_PRIOR = 0x21,
	K_NEXT = 0x22,
	K_END = 0x23,
	K_HOME = 0x24,
	K_LEFT = 0x25,
	K_UP = 0x26,
	K_RIGHT = 0x27,
	K_DOWN = 0x28,
	K_SELECT = 0x29,
	K_PRINT = 0x2A,
	K_PRINTSCREEN = 0x2C,
	K_INSERT = 0x2D,
	K_DELETE = 0x2E,
	K_LWIN = 0x5B,
	K_RWIN = 0x5C,
	K_APPS = 0x5D,
	K_SLEEP = 0x5F,
	K_NUMPAD0 = 0x60,
	K_NUMPAD1 = 0x61,
	K_NUMPAD2 = 0x62,
	K_NUMPAD3 = 0x63,
	K_NUMPAD4 = 0x64,
	K_NUMPAD5 = 0x65,
	K_NUMPAD6 = 0x66,
	K_NUMPAD7 = 0x67,
	K_NUMPAD8 = 0x68,
	K_NUMPAD9 = 0x69,
	K_MULTIPLY = 0x6A,
	K_ADD = 0x6B,
	K_SEPARATOR = 0x6C,
	K_SUBTRACT = 0x6D,
	K_DECIMAL = 0x6E,
	K_DIVIDE = 0x6F,
	K_F1 = 0x70,
	K_F2 = 0x71,
	K_F3 = 0x72,
	K_F4 = 0x73,
	K_F5 = 0x74,
	K_F6 = 0x75,
	K_F7 = 0x76,
	K_F8 = 0x77,
	K_F9 = 0x78,
	K_F10 = 0x79,
	K_F11 = 0x7A,
	K_F12 = 0x7B,
	K_F13 = 0x7C,
	K_F14 = 0x7D,
	K_F15 = 0x7E,
	K_F16 = 0x7F,
	K_F17 = 0x80,
	K_F18 = 0x81,
	K_F19 = 0x82,
	K_F20 = 0x83,
	K_F21 = 0x84,
	K_F22 = 0x85,
	K_F23 = 0x86,
	K_F24 = 0x87,
	K_NUMLOCK = 0x90,
	K_SCROLL = 0x91
};

enum ModifierKey
{
	KEY_SHIFT = 1 << 0,
	KEY_CONTROL = 1 << 1,
	KEY_ALT = 1 << 2
};
