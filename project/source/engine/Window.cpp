#include "Pch.h"
#include "Core.h"
#include "Input.h"
#include "Window.h"
#include "WindowsIncl.h"

Window::Window() : closed(false), active(false), mouse_move(0,0)
{
}

void Window::Init(cstring title, const INT2& _size, bool _fullscreen)
{
	Info("Creating window.");
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
	hwnd = CreateWindowEx(0, "RogueSaviorWndCls", title, fullscreen ? WS_POPUPWINDOW : WS_OVERLAPPEDWINDOW, 0, 0, real_size.x, real_size.y,
		nullptr, nullptr, GetModuleHandle(nullptr), this);
	if(!hwnd)
		throw Format("Failed to create window (%d).", GetLastError());

	// position window on center
	if(!fullscreen)
	{
		MoveWindow(hwnd,
			(GetSystemMetrics(SM_CXSCREEN) - real_size.x) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - real_size.y) / 2,
			real_size.x, real_size.y, false);
	}

	// show window
	ShowWindow(hwnd, SW_SHOW);
}

long Window::HandleEventStatic(HWND hwnd, uint msg, uint wParam, long lParam)
{
	Window* wnd;

	if(msg == WM_NCCREATE)
	{
		wnd = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

		SetLastError(0);
		if(!SetWindowLongPtr(hwnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(wnd)))
		{
			if(GetLastError() != 0)
				return FALSE;
		}
	}
	else
		wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWL_USERDATA));

	if(wnd)
		return wnd->HandleEvent(hwnd, msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

long Window::HandleEvent(HWND in_hwnd, uint msg, uint wParam, long lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		closed = true;
		return 0;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		Input.Process(wParam, true);
		return 0;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		Input.Process(wParam, false);
		return 0;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
		{
			byte key;
			int ret;
			MsgToKey(msg, wParam, key, ret);
			Input.Process(key, true);
			return ret;
		}

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		{
			byte key;
			int ret;
			MsgToKey(msg, wParam, key, ret);
			Input.Process(key, false);
			return ret;
		}

	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
		{
			byte key;
			int ret = 0;
			MsgToKey(msg, wParam, key, ret);
			Input.ProcessDoubleClick(key);
			return ret;
		}

	case WM_MOUSEWHEEL:
		Input.UpdateMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		return 0;
	}

	return DefWindowProc(in_hwnd, msg, wParam, lParam);
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
	ShowWindow(hwnd, SW_HIDE);
}

void Window::ShowError(cstring msg)
{
	assert(msg);

	ShowWindow(hwnd, SW_HIDE);
	UpdateActivity(false);
	MessageBox(nullptr, msg, nullptr, MB_OK | MB_ICONERROR | MB_APPLMODAL);
}

bool Window::Update()
{
	// handle messages
	MSG msg = { 0 };
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(msg.message == WM_QUIT)
			return false;
	}

	// update activity state
	HWND foreground = GetForegroundWindow();
	bool is_active = (foreground == hwnd);
	bool was_active = active;
	UpdateActivity(is_active);
	
	// handle cursor movment
	if(active)
	{
		if(was_active)
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hwnd, &pt);
			INT2 dif = INT2(pt.x, pt.y) - size / 2;
			Input.UpdateMouse(dif);
		}
		POINT center = { size.x / 2, size.y / 2 };
		ClientToScreen(hwnd, &center);
		SetCursorPos(center.x, center.y);
	}

	return !closed;
}

void Window::MsgToKey(uint msg, uint wParam, byte& key, int& result)
{
	result = 0;

	switch(msg)
	{
	default:
		assert(0);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		key = VK_LBUTTON;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		key = VK_RBUTTON;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		key = VK_MBUTTON;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		key = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		result = TRUE;
		break;
	}
}

void Window::UpdateActivity(bool is_active)
{
	if(is_active == active)
		return;
	active = is_active;
	if(active)
	{
		ShowCursor(FALSE);
		POINT center = { size.x / 2, size.y / 2 };
		ClientToScreen(hwnd, &center);
		SetCursorPos(center.x, center.y);
	}
	else
	{
		ShowCursor(TRUE);
	}
}
