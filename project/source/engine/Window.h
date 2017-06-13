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
	bool Update();
	bool IsActive() { return active; }
	void ShowError(cstring msg); 

private:
	void AdjustWindowSize();
	long HandleEvent(HWND in_hwnd, uint msg, uint wParam, long lParam);
	void MsgToKey(uint msg, uint wParam, byte& key, int& result);
	void UpdateActivity(bool is_active);

	HWND hwnd;
	INT2 size, real_size, mouse_move;
	bool fullscreen, closed, active;
};
