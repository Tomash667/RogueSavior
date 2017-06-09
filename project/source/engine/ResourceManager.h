#pragma once

class Mesh;
class Resource;
class Texture;

class ResourceManager
{
public:
	Mesh* LoadMesh(cstring path);
	Texture* LoadTexture(cstring path);

private:
	std::map<string, Resource*> resources;
};
