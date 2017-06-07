#include "Pch.h"
#include "Core.h"
#include "Window.h"
#include "WindowsIncl.h"

Window::Window()
{

}

void Window::Init(cstring title, const INT2& size, bool fullscreen)
{
	// register window class
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		[this](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT { return HandleEvent(hwnd, msg, wParam, lParam); },
		0, 0, GetModuleHandle(nullptr), LoadIcon(GetModuleHandle(nullptr), "Icon"), LoadCursor(nullptr, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH),
		nullptr, "RogueSaviorWndCls", nullptr
	};
	if(!RegisterClassEx(&wc))
		throw Format("Failed to register window class (%d).", GetLastError());
}
