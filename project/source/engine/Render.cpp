#include "Pch.h"
#include "Core.h"
#include "Gui.h"
#include "Render.h"
#include "Scene.h"
#include "DirectXIncl.h"

#define DISPLAY_FORMAT D3DFMT_X8R8G8B8
#define BACKBUFFER_FORMAT D3DFMT_A8R8G8B8
#define ZBUFFER_FORMAT D3DFMT_D24S8

Render::Render() : d3d(nullptr), device(nullptr), scene(nullptr), lost_device(false), resources_released(false), effect_pool(nullptr), e_mesh(nullptr), e_animated(nullptr),
e_gui(nullptr)
{

}

Render::~Render()
{
	SafeRelease(e_mesh);
	SafeRelease(e_animated);
	SafeRelease(effect_pool);
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

	CreateVertexDeclarations();
	LoadShaders();
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
	if(caps.VertexShaderVersion >= D3DVS_VERSION(2, 0) && caps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	{
		shader_version = 3;
		Info("Using 3.0 shaders.");
	}
	else
	{
		shader_version = 2;
		Info("Using 2.0 shaders.");
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

void Render::CreateVertexDeclarations()
{
	const D3DVERTEXELEMENT9 Default[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0 },
		{ 0, 12,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,		0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0 },
		D3DDECL_END()
	};
	V(device->CreateVertexDeclaration(Default, &vertex_decl[VDI_DEFAULT]));

	const D3DVERTEXELEMENT9 Animated[] = {
		{ 0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0 },
		{ 0,	12,	D3DDECLTYPE_FLOAT1,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDWEIGHT,	0 },
		{ 0,	16,	D3DDECLTYPE_UBYTE4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },
		{ 0,	20,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,		0 },
		{ 0,	32,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0 },
		D3DDECL_END()
	};
	V(device->CreateVertexDeclaration(Animated, &vertex_decl[VDI_ANIMATED]));

	const D3DVERTEXELEMENT9 Tangents[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0 },
		{ 0, 12,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,		0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0 },
		{ 0,	32,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TANGENT,		0 },
		{ 0,	44,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BINORMAL,		0 },
		D3DDECL_END()
	};
	V(device->CreateVertexDeclaration(Tangents, &vertex_decl[VDI_TANGENT]));

	const D3DVERTEXELEMENT9 AnimatedTangents[] = {
		{ 0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0 },
		{ 0,	12,	D3DDECLTYPE_FLOAT1,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDWEIGHT,	0 },
		{ 0,	16,	D3DDECLTYPE_UBYTE4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },
		{ 0,	20,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,		0 },
		{ 0,	32,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0 },
		{ 0,	40,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TANGENT,		0 },
		{ 0,	52,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BINORMAL,		0 },
		D3DDECL_END()
	};
	V(device->CreateVertexDeclaration(AnimatedTangents, &vertex_decl[VDI_ANIMATED_TANGENT]));

	const D3DVERTEXELEMENT9 Pos[] = {
		{ 0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0 },
		D3DDECL_END()
	};
	V(device->CreateVertexDeclaration(Pos, &vertex_decl[VDI_POS]));
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
	assert(scene);

	HRESULT hr = device->TestCooperativeLevel();
	if(hr != D3D_OK)
	{
		if(!lost_device)
		{
			lost_device = true;
			Info("Lost directx device.");
		}
		if(hr == D3DERR_DEVICELOST)
		{
			// device is lost, can't reset
			Sleep(10);
			return;
		}
		else if(hr == D3DERR_DEVICENOTRESET)
		{
			// try reset device
			if(!Reset())
			{
				Sleep(10);
				return;
			}
		}
		else
			throw Format("Lost directx device (%d).", hr);
	}

	V(device->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET | D3DCLEAR_STENCIL, scene->GetClearColor(), 1.f, 0));
	V(device->BeginScene());

	scene->Draw();
	if(gui)
		gui->Draw();

	V(device->EndScene());
	hr = device->Present(nullptr, nullptr, hwnd, nullptr);
	if(FAILED(hr))
	{
		if(hr == D3DERR_DEVICELOST)
		{
			lost_device = true;
			Info("Lost directx device.");
		}
		else
			throw Format("Failed to present screen (%d).", hr);
	}
}

ID3DXEffect* Render::GetShader(Shader shader)
{
	switch(shader)
	{
	case Shader::Mesh:
		return e_mesh;
	case Shader::Animated:
		return e_animated;
	default:
		return nullptr;
	}
}

void Render::LoadShaders()
{
	V(D3DXCreateEffectPool(&effect_pool));

	e_mesh = LoadShader("data/shaders/mesh.fx", effect_pool);
	e_animated = LoadShader("data/shaders/mesh.fx", effect_pool, "ANIMATED");
}

ID3DXEffect* Render::LoadShader(cstring name, ID3DXEffectPool* pool, cstring param)
{
	assert(name);

	// load shader code
	string code;
	if(!LoadFileToString(name, code))
		throw Format("Missing shader '%s'.", name);

	// set macros
	D3DXMACRO macros[4] = {
		"VS_VERSION", shader_version == 3 ? "vs_3_0" : "vs_2_0",
		"PS_VERSION", shader_version == 3 ? "ps_3_0" : "ps_2_0",
		nullptr, nullptr,
		nullptr, nullptr
	};
	if(param)
	{
		macros[2].Name = param;
		macros[2].Definition = "1";
	}

	// create shader compiler
	const DWORD flags =
#ifdef _DEBUG
		D3DXSHADER_DEBUG | D3DXSHADER_OPTIMIZATION_LEVEL1;
#else
		D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif
	ID3DXBuffer* errors = nullptr;
	ID3DXEffectCompiler* compiler = nullptr;
	HRESULT hr = hr = D3DXCreateEffectCompiler(code.c_str(), code.length(), macros, nullptr, flags, &compiler, &errors);
	if(FAILED(hr))
	{
		cstring str;
		if(errors)
			str = (cstring)errors->GetBufferPointer();
		else
		{
			switch(hr)
			{
			case D3DXERR_INVALIDDATA:
				str = "Invalid data.";
				break;
			case D3DERR_INVALIDCALL:
				str = "Invalid call.";
				break;
			case E_OUTOFMEMORY:
				str = "Out of memory.";
				break;
			case ERROR_MOD_NOT_FOUND:
			case 0x8007007e:
				str = "Can't find module (missing d3dcompiler_43.dll?).";
				break;
			default:
				str = "Unknown error.";
				break;
			}
		}

		cstring msg = Format("Failed to compile shader '%s' (%d).\n%s", name, hr, str);

		SafeRelease(errors);

		throw msg;
	}
	SafeRelease(errors);

	// compile shader
	ID3DXBuffer* effect_buffer = nullptr;
	hr = compiler->CompileEffect(flags, &effect_buffer, &errors);
	if(FAILED(hr))
	{
		cstring msg = Format("Failed to compile effect '%s' (%d).\n%s", name, hr,
			errors ? (cstring)errors->GetBufferPointer() : "No errors information.");

		SafeRelease(errors);
		SafeRelease(effect_buffer);
		SafeRelease(compiler);

		throw msg;
	}
	SafeRelease(errors);
	
	// create effect from effect buffer
	ID3DXEffect* effect = nullptr;
	hr = D3DXCreateEffect(device, effect_buffer->GetBufferPointer(), effect_buffer->GetBufferSize(),
		macros, nullptr, flags, pool, &effect, &errors);
	if(FAILED(hr))
	{
		cstring msg = Format("Failed to create effect '%s' (%d).\n%s", name, hr,
			errors ? (cstring)errors->GetBufferPointer() : "No errors information.");

		SafeRelease(errors);
		SafeRelease(effect_buffer);
		SafeRelease(compiler);

		throw msg;
	}

	// free directx stuff
	SafeRelease(errors);
	SafeRelease(effect_buffer);
	SafeRelease(compiler);

	return effect;
}

void Render::OnLostDevice()
{
	if(resources_released)
		return;

	if(e_mesh)
		V(e_mesh->OnLostDevice());
	if(e_animated)
		V(e_animated->OnLostDevice());
	if(e_gui)
		V(e_gui->OnLostDevice());

	resources_released = true;
}

void Render::OnResetDevice()
{
	if(e_mesh)
		V(e_mesh->OnResetDevice());
	if(e_animated)
		V(e_animated->OnResetDevice());
	if(e_gui)
		V(e_gui->OnResetDevice());

	resources_released = false;
}

bool Render::Reset()
{
	Info("Reseting device.");
	OnLostDevice();

	// gather present params
	D3DPRESENT_PARAMETERS d3dpp = { 0 };
	GatherParams(d3dpp);

	// try to reset
	HRESULT hr = device->Reset(&d3dpp);
	if(FAILED(hr))
	{
		if(hr != D3DERR_DEVICELOST)
			throw Format("Engine: Failed to reset directx device (%d)!", hr);
		else
		{
			if(!lost_device)
			{
				lost_device = true;
				Info("Lost directx device.");
			}
			Warn("Failed to reset device.");
			return false;
		}
	}

	Info("Device reseted.");
	lost_device = false;
	OnResetDevice();

	return true;
}
