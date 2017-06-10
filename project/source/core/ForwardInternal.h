#pragma once

// WINAPI handles
#define DECLARE_HANDLE(name) struct name##__; typedef name##__ *name
DECLARE_HANDLE(HWND);
#undef DECLARE_HANDLE
typedef void *HANDLE;

// DirectX
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DTexture9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
typedef struct _D3DPRESENT_PARAMETERS_ D3DPRESENT_PARAMETERS;
