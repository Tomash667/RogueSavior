#pragma once

struct Mesh;
struct MeshInstance;

struct SceneNode
{
	Mesh* mesh;
	MeshInstance* mesh_inst;
	VEC3 pos, rot, tint;

	SceneNode(Mesh* mesh) : mesh(mesh), mesh_inst(nullptr), pos(0, 0, 0), rot(0, 0, 0), tint(1, 1, 1)
	{

	}
};
