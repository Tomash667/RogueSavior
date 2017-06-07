#pragma once

class Window
{
public:
	Window();

	void Init(cstring title, const INT2& size, bool fullscreen);

private:
	long HandleEvent(void* in_hwnd, uint msg, uint wParam, long lParam);

	void* hwnd;
};
