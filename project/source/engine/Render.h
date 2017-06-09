#pragma once

#include "ForwardInternal.h"

class Render
{
public:
	Render();
	~Render();

	void Init(HWND hwnd, const INT2& window_size, int hz, bool fullscreen);
	void Draw();

private:
	void CheckCompability();
	void CreateDevice();
	void GatherParams(D3DPRESENT_PARAMETERS& d3dpp);

	IDirect3D9* d3d;
	IDirect3DDevice9* device;
	HWND hwnd;
	INT2 window_size;
	int hz;
	bool fullscreen;
};
