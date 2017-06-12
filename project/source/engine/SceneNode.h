#pragma once

struct Mesh;
struct MeshInstance;

struct SceneNode
{
	Mesh* mesh;
	MeshInstance* mesh_inst;
	VEC3 pos;
};
