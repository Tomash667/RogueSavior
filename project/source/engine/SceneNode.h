#pragma once

struct Mesh;
struct MeshInstance;

struct SceneNode
{
	Mesh* mesh;
	MeshInstance* mesh_inst;
	VEC3 pos;

	SceneNode(Mesh* mesh) : mesh(mesh), mesh_inst(nullptr), pos(0, 0, 0)
	{

	}
};
