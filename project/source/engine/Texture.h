#pragma once

#include "ForwardInternal.h"
#include "Resource.h"

struct Texture : public Resource
{
	IDirect3DTexture9* tex;
};
