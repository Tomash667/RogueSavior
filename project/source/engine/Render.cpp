#include "Pch.h"
#include "Core.h"
#include "Render.h"
#include "DirectXIncl.h"

#define DISPLAY_FORMAT D3DFMT_X8R8G8B8
#define BACKBUFFER_FORMAT D3DFMT_A8R8G8B8
#define ZBUFFER_FORMAT D3DFMT_D24S8

template<typename T>
inline void SafeRelease(T item)
{
	if(item)
		item->Release();
}

Render::Render() : d3d(nullptr), device(nullptr)
{

}

Render::~Render()
{
	SafeRelease(device);
	SafeRelease(d3d);
}

void Render::Init(HWND _hwnd, const INT2& _window_size, int _hz, bool _fullscreen)
{
	Info("Creating render.");

	hwnd = _hwnd;
	window_size = _window_size;
	hz = _hz;
	fullscreen = _fullscreen;

	// create d3d object
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d)
		throw "Failed to create direct3d object.";

	CheckCompability();

	CreateDevice();
}

void Render::CheckCompability()
{
	HRESULT hr;

	// check shaders version
	D3DCAPS9 caps;
	hr = d3d->GetDeviceCaps(0, D3DDEVTYPE_HAL, &caps);
	assert(SUCCEEDED(hr));
	Info("Supported shader version vertex: %d.%d, pixel: %d.%d.",
		D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion),
		D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));
	if(D3DVS_VERSION(2, 0) > caps.VertexShaderVersion || D3DPS_VERSION(2, 0) > caps.PixelShaderVersion)
	{
		throw Format("Unsupported and too old graphic card. This game require vertex and pixel shader version 2.0+. "
			"Your card supports:\nVertex shader: %d.%d\nPixel shader: %d.%d",
			D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion),
			D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));
	}

	// check texture types
	hr = d3d->CheckDeviceType(0, D3DDEVTYPE_HAL, DISPLAY_FORMAT, BACKBUFFER_FORMAT, fullscreen ? FALSE : TRUE);
	if(FAILED(hr))
		throw Format("Unsupported backbuffer type %s for display %s! (%d)", STRING(BACKBUFFER_FORMAT), STRING(DISPLAY_FORMAT), hr);

	hr = d3d->CheckDeviceFormat(0, D3DDEVTYPE_HAL, DISPLAY_FORMAT, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, ZBUFFER_FORMAT);
	if(FAILED(hr))
		throw Format("Unsupported depth buffer type %s for display %s! (%d)", STRING(ZBUFFER_FORMAT), STRING(DISPLAY_FORMAT), hr);

	hr = d3d->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, DISPLAY_FORMAT, D3DFMT_A8R8G8B8, ZBUFFER_FORMAT);
	if(FAILED(hr))
		throw Format("Unsupported render target D3DFMT_A8R8G8B8 with display %s and depth buffer %s! (%d)",
			STRING(DISPLAY_FORMAT), STRING(BACKBUFFER_FORMAT), hr);
}

void Render::CreateDevice()
{
	// gather params
	D3DPRESENT_PARAMETERS d3dpp = { 0 };
	GatherParams(d3dpp);

	// create d3d device in one of 3 modes
	const DWORD mode[] = {
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		D3DCREATE_MIXED_VERTEXPROCESSING,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING
	};
	const cstring mode_str[] = {
		"hardware",
		"mixed",
		"software"
	};

	DWORD hr;
	for(uint i = 0; i < 3; ++i)
	{
		hr = d3d->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, mode[i], &d3dpp, &device);
		if(SUCCEEDED(hr))
		{
			Info("Created direct3d device in %s mode.", mode_str[i]);
			break;
		}
	}

	if(FAILED(hr))
		throw Format("Failed to create direct3d device (%d).", hr);
}

void Render::GatherParams(D3DPRESENT_PARAMETERS& d3dpp)
{
	d3dpp.Windowed = !fullscreen;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = BACKBUFFER_FORMAT;
	d3dpp.BackBufferWidth = window_size.x;
	d3dpp.BackBufferHeight = window_size.y;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.AutoDepthStencilFormat = ZBUFFER_FORMAT;
	d3dpp.Flags = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.FullScreen_RefreshRateInHz = (fullscreen ? hz : 0);
}

void Render::Draw()
{
	/*HRESULT hr = device->TestCooperativeLevel();
	if(hr != D3D_OK)
	{
		lost_device = true;
		if(hr == D3DERR_DEVICELOST)
		{
			// urz¹dzenie utracone, nie mo¿na jeszcze resetowaæ
			Sleep(1);
			return;
		}
		else if(hr == D3DERR_DEVICENOTRESET)
		{
			// mo¿na resetowaæ
			if(!Reset(false))
			{
				Sleep(1);
				return;
			}
		}
		else
			throw Format("Lost directx device (%d)!", hr);
	}*/

	device->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET | D3DCLEAR_STENCIL, 0xFF0000FF, 1.f, 0);

	//	hr = 
	device->Present(nullptr, nullptr, hwnd, nullptr);
	/*	if(FAILED(hr))
		{
			if(hr == D3DERR_DEVICELOST)
				lost_device = true;
			else
				throw Format("Engine: Failed to present screen (%d)!", hr);
		}
	}*/
}
