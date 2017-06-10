#pragma once

#include "Resource.h"
#include "VertexDeclaration.h"

class ResourceManager;
struct Texture;

struct Mesh : public Resource
{
	enum Flags
	{
		F_TANGENTS = 1 << 0,
		F_ANIMATED = 1 << 1,
		F_STATIC = 1 << 2,
		F_PHYSICS = 1 << 3,
		F_SPLIT = 1 << 4
	};

	struct Header
	{
		char format[4];
		byte version, flags;
		word n_verts, n_tris, n_subs, n_bones, n_anims, n_points, n_groups;
		float radius;
		BOX bbox;
		VEC3 cam_pos, cam_target, cam_up;
	};

	struct Submesh
	{
		word first; // pierwszy strójk¹t do narysowania
		word tris; // ile trójk¹tów narysowaæ
		word min_ind; // odpowiednik parametru DrawIndexedPrimitive - MinIndex (tylko wyra¿ony w trójk¹tach)
		word n_ind; // odpowiednik parametru DrawIndexedPrimitive - NumVertices (tylko wyra¿ony w trójk¹tach)
		string name;//, normal_name, specular_name;
		Texture* tex, *tex_normal, *tex_specular;
		VEC3 specular_color;
		float specular_intensity;
		int specular_hardness;
		float normal_factor, specular_factor, specular_color_factor;

		static const uint MIN_SIZE = 10;
	};

	struct BoneGroup
	{
		word parent;
		string name;
		vector<byte> bones;

		static const uint MIN_SIZE = 4;
	};

	struct Bone
	{
		word id;
		word parent;
		MATRIX mat;
		string name;
		vector<word> childs;

		static const uint MIN_SIZE = 51;
	};

	struct KeyframeBone
	{
		VEC3 pos;
		QUAT rot;
		float scale;

		void Mix(MATRIX& out, const MATRIX& mul) const;
		static void Interpolate(KeyframeBone& out, const KeyframeBone& k, const KeyframeBone& k2, float t);
	};

	struct Keyframe
	{
		float time;
		vector<KeyframeBone> bones;
	};

	struct Animation
	{
		string name;
		float length;
		word n_frames;
		vector<Keyframe> frames;

		static const uint MIN_SIZE = 7;

		int GetFrameIndex(float time, bool& hit);
	};

	struct Point
	{
		enum Type : word
		{
			OTHER,
			SPHERE,
			BOX
		};

		string name;
		MATRIX mat;
		VEC3 rot;
		word bone;
		Type type;
		VEC3 size;

		static const uint MIN_SIZE = 73;

		bool IsSphere() const { return type == SPHERE; }
		bool IsBox() const { return type == BOX; }
	};

	struct Split
	{
		VEC3 pos;
		float radius;
		BOX box;
	};

	Header head;
	IDirect3DVertexBuffer9* vb;
	IDirect3DIndexBuffer9* ib;
	VertexDeclarationId vertex_decl;
	uint vertex_size;
	vector<Submesh> subs;
	vector<Bone> bones;
	vector<Animation> anims;
	vector<MATRIX> model_to_bone;
	vector<Point> attach_points;
	vector<BoneGroup> groups;
	vector<Split> splits;

	void Load(cstring path, IDirect3DDevice9* device, ResourceManager& res_mgr);
	void SetupBoneMatrices();
};
