#pragma once

class Window
{
public:
	Window();

	void Hide();
	void Init(cstring title, const INT2& size, bool fullscreen);
	static long HandleEventStatic(Handle in_hwnd, uint msg, uint wParam, long lParam);
	bool HandleMessages();
	void ShowError(cstring msg); 

private:
	void AdjustWindowSize();
	long HandleEvent(Handle in_hwnd, uint msg, uint wParam, long lParam);
	void MsgToKey(uint msg, uint wParam, byte& key, int& result);

	Handle hwnd;
	INT2 size, real_size;
	bool fullscreen, closed;
};
