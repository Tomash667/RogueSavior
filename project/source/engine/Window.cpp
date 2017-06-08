#include "Pch.h"
#include "Core.h"
#include "Input.h"
#include "Window.h"
#include "WindowsIncl.h"

Window::Window() : closed(false)
{
}

void Window::Init(cstring title, const INT2& _size, bool _fullscreen)
{
	size = _size;
	fullscreen = _fullscreen;

	// register window class
	auto wnd = this;
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, (WNDPROC)HandleEventStatic,
		0, 0, GetModuleHandle(nullptr), LoadIcon(GetModuleHandle(nullptr), "Icon"), LoadCursor(nullptr, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH),
		nullptr, "RogueSaviorWndCls", nullptr
	};
	if(!RegisterClassEx(&wc))
		throw Format("Failed to register window class (%d).", GetLastError());

	// create window
	AdjustWindowSize();
	hwnd = (Handle)CreateWindowEx(0, "RogueSaviorWndCls", title, fullscreen ? WS_POPUPWINDOW : WS_OVERLAPPEDWINDOW, 0, 0, real_size.x, real_size.y,
		nullptr, nullptr, GetModuleHandle(nullptr), this);
	if(!hwnd)
		throw Format("Failed to create window (%d).", GetLastError());

	// position window on center
	if(!fullscreen)
	{
		MoveWindow((HWND)hwnd,
			(GetSystemMetrics(SM_CXSCREEN) - real_size.x) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - real_size.y) / 2,
			real_size.x, real_size.y, false);
	}

	// show window
	ShowWindow((HWND)hwnd, SW_SHOW);
}

long Window::HandleEventStatic(Handle hwnd, uint msg, uint wParam, long lParam)
{
	Window* wnd;

	if(msg == WM_NCCREATE)
	{
		wnd = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

		SetLastError(0);
		if(!SetWindowLongPtr((HWND)hwnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(wnd)))
		{
			if(GetLastError() != 0)
				return FALSE;
		}
	}
	else
		wnd = reinterpret_cast<Window*>(GetWindowLongPtr((HWND)hwnd, GWL_USERDATA));

	if(wnd)
		return wnd->HandleEvent(hwnd, msg, wParam, lParam);
	else
		return DefWindowProc((HWND)hwnd, msg, wParam, lParam);
}

long Window::HandleEvent(Handle in_hwnd, uint msg, uint wParam, long lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		closed = true;
		return 0;

	case WM_ACTIVATE:
		{
			bool active = (wParam != WA_INACTIVE);
			if(!active)
				Input.ReleaseKeys();
		}
		return 0;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		Input.Process(wParam, true);
		return 0;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		Input.Process(wParam, false);
		return 0;
	}

	return DefWindowProc((HWND)in_hwnd, msg, wParam, lParam);
}

void Window::AdjustWindowSize()
{
	if(!fullscreen)
	{
		RECT rect = { 0 };
		rect.right = size.x;
		rect.bottom = size.y;

		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

		real_size.x = abs(rect.left - rect.right);
		real_size.y = abs(rect.top - rect.bottom);
	}
	else
		real_size = size;
}

void Window::Hide()
{
	ShowWindow((HWND)hwnd, SW_HIDE);
}

void Window::ShowError(cstring msg)
{
	assert(msg);

	ShowWindow((HWND)hwnd, SW_HIDE);
	ShowCursor(TRUE);
	MessageBox(nullptr, msg, nullptr, MB_OK | MB_ICONERROR | MB_APPLMODAL);
}

bool Window::HandleMessages()
{
	MSG msg = { 0 };
	
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(msg.message == WM_QUIT)
			return false;
	}

	return !closed;
}
