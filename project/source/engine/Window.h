#pragma once

#include "ForwardInternal.h"

class Window
{
public:
	Window();

	void Hide();
	void Init(cstring title, const INT2& size, bool fullscreen);
	HWND GetHandle() { return hwnd; }
	static long HandleEventStatic(HWND in_hwnd, uint msg, uint wParam, long lParam);
	bool HandleMessages();
	void ShowError(cstring msg); 

private:
	void AdjustWindowSize();
	long HandleEvent(HWND in_hwnd, uint msg, uint wParam, long lParam);
	void MsgToKey(uint msg, uint wParam, byte& key, int& result);

	HWND hwnd;
	INT2 size, real_size;
	bool fullscreen, closed;
};
