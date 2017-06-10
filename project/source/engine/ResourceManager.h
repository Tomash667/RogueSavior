#pragma once

#include "ForwardInternal.h"
#include "Resource.h"

struct Mesh;
struct Texture;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void AddDir(cstring path);
	void Init(IDirect3DDevice9* device);
	Mesh* LoadMesh(cstring path);
	Texture* LoadTexture(cstring path);

private:
	Resource* CreateResource(Resource::Type type);
	Resource* GetResource(cstring filename, Resource::Type type);

	IDirect3DDevice9* device;
	std::map<string, Resource*> resources;
	std::map<string, Resource::Type> exts;
};
