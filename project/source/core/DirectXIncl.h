#pragma once

#define D3D_DEBUG_INFO
#include <d3dx9.h>

#ifdef _DEBUG
#	define V(x) { HRESULT _hr; assert(SUCCEEDED(_hr = (x))); }
#else
#	define V(x) x
#endif
