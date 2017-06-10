#include "Pch.h"
#include "Core.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "DirectXIncl.h"

ResourceManager::ResourceManager() : device(nullptr)
{

}

ResourceManager::~ResourceManager()
{
	for(auto& r : resources)
		delete r.second;
}

void ResourceManager::AddDir(cstring path)
{
	FindFiles(Format("%s/*.*", path), [this, path](const FindData& data)
	{
		if(data.is_dir)
		{
			AddDir(data.filename.c_str());
			return true;
		}
		else
		{
			string ext = GetExt(data.filename);
			auto it = exts.find(ext);
			if(it == exts.end())
				return true;
			auto res = CreateResource(it->second);
			res->type = it->second;
			res->state = Resource::NOT_LOADED;
			cstring resource_path = Format("%s/%s", path, data.filename.c_str());
			resources[resource_path] = res;
			return true;
		}
	});
}

void ResourceManager::Init(IDirect3DDevice9* _device)
{
	assert(_device);
	device = _device;

	exts["bmp"] = Resource::TEXTURE;
	exts["jpg"] = Resource::TEXTURE;
	exts["tga"] = Resource::TEXTURE;
	exts["png"] = Resource::TEXTURE;
	exts["dds"] = Resource::TEXTURE;
	exts["ppm"] = Resource::TEXTURE;
	exts["dib"] = Resource::TEXTURE;
	exts["hdr"] = Resource::TEXTURE;
	exts["pfm"] = Resource::TEXTURE;
	exts["qmsh"] = Resource::MESH;
}

Mesh* ResourceManager::LoadMesh(cstring path)
{
	assert(path);

	auto res = GetResource(path, Resource::MESH);
	if(!res)
		throw Format("Missing mesh '%s'.", path);

	auto mesh = static_cast<Mesh*>(res);
	if(mesh->state != Resource::LOADED)
	{
		try
		{
			mesh->Load(path, device, *this);
			mesh->state = Resource::LOADED;
		}
		catch(cstring err)
		{
			throw Format("Failed to load mesh '%s': %s", path, err);
		}
	}

	return mesh;
}

Texture* ResourceManager::LoadTexture(cstring path)
{
	assert(path);

	auto res = GetResource(path, Resource::TEXTURE);
	if(!res)
		throw Format("Missing texture '%s'.", path);

	auto tex = static_cast<Texture*>(res);
	if(tex->state != Resource::LOADED)
	{
		HRESULT hr = D3DXCreateTextureFromFile(device, path, &tex->tex);
		if(!SUCCEEDED(hr))
			throw Format("Failed to load texture '%s' (%d).", path, hr);
		tex->state = Resource::LOADED;
	}

	return tex;
}

Resource* ResourceManager::CreateResource(Resource::Type type)
{
	switch(type)
	{
	case Resource::MESH:
		return new Mesh;
	case Resource::TEXTURE:
		return new Texture;
	default:
		assert(0);
		return nullptr;
	}
}

Resource* ResourceManager::GetResource(cstring path, Resource::Type type)
{
	auto it = resources.find(path);
	if(it != resources.end() && it->second->type == type)
		return it->second;
	else
		return nullptr;
}
