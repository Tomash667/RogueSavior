#include "Pch.h"
#include "Core.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "DirectXIncl.h"

void Mesh::Load(cstring path, IDirect3DDevice9* device, ResourceManager& res_mgr)
{
	assert(path && device);

	// open file
	FileReader f(path);
	if(!f)
		throw "Can't open file.";
	string dir = GetParentDir(path);

	// header
	if(!f.Read(head))
		throw "Failed to read file header.";
	if(memcmp(head.format, "QMSH", 4) != 0)
		throw Format("Invalid file signature '%.4s'.", head.format);
	if(head.version != 19)
		throw Format("Invalid file version '%d'.", head.version);
	if(head.n_bones >= 32)
		throw Format("Too many bones (%d).", head.n_bones);
	if(head.n_subs == 0)
		throw "Missing model mesh.";
	if(IS_SET(head.flags, F_ANIMATED) && !IS_SET(head.flags, F_STATIC))
	{
		if(head.n_bones == 0)
			throw "No bones.";
		if(head.n_groups == 0)
			throw "No bone groups.";
	}
	
	// set vertex size & fvf
	if(IS_SET(head.flags, F_PHYSICS))
	{
		vertex_decl = VDI_POS;
		vertex_size = sizeof(VPos);
	}
	else
	{
		vertex_size = sizeof(VEC3);
		if(IS_SET(head.flags, F_ANIMATED))
		{
			if(IS_SET(head.flags, F_TANGENTS))
			{
				vertex_decl = VDI_ANIMATED_TANGENT;
				vertex_size = sizeof(VAnimatedTangent);
			}
			else
			{
				vertex_decl = VDI_ANIMATED;
				vertex_size = sizeof(VAnimated);
			}
		}
		else
		{
			if(IS_SET(head.flags, F_TANGENTS))
			{
				vertex_decl = VDI_TANGENT;
				vertex_size = sizeof(VTangent);
			}
			else
			{
				vertex_decl = VDI_DEFAULT;
				vertex_size = sizeof(VDefault);
			}
		}
	}

	// ensure vertices size
	uint size = vertex_size * head.n_verts;
	if(!f.Ensure(size))
		throw "Failed to read vertex buffer.";

	// create vertex buffer
	HRESULT hr = device->CreateVertexBuffer(size, 0, 0, D3DPOOL_MANAGED, &vb, nullptr);
	if(FAILED(hr))
		throw Format("Failed to create vertex buffer (%d).", hr);

	// read
	void* ptr;
	V(vb->Lock(0, size, &ptr, 0));
	f.Read(ptr, size);
	V(vb->Unlock());

	// ----- triangles
	// ensure size
	size = sizeof(word) * head.n_tris * 3;
	if(!f.Ensure(size))
		throw "Failed to read index buffer.";

	// create index buffer
	hr = device->CreateIndexBuffer(size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, nullptr);
	if(FAILED(hr))
		throw Format("Failed to create index buffer (%d).", hr);

	// read
	V(ib->Lock(0, size, &ptr, 0));
	f.Read(ptr, size);
	V(ib->Unlock());

	// ----- submeshes
	size = Submesh::MIN_SIZE * head.n_subs;
	if(!f.Ensure(size))
		throw "Failed to read submesh data.";
	subs.resize(head.n_subs);

	for(word i = 0; i<head.n_subs; ++i)
	{
		Submesh& sub = subs[i];

		f.Read(sub.first);
		f.Read(sub.tris);
		f.Read(sub.min_ind);
		f.Read(sub.n_ind);
		f.ReadString1(sub.name);

		// texture
		f.ReadString1();
		if(BUF[0])
			sub.tex = res_mgr.LoadTexture(Format("%s/%s", dir.c_str(), BUF));
		else
			sub.tex = nullptr;

		// specular values
		f.Read(sub.specular_color);
		f.Read(sub.specular_intensity);
		f.Read(sub.specular_hardness);

		// normalmap
		if(IS_SET(head.flags, F_TANGENTS))
		{
			f.ReadString1();
			if(BUF[0])
			{
				sub.tex_normal = res_mgr.LoadTexture(Format("%s/%s", dir.c_str(), BUF));
				f.Read(sub.normal_factor);
			}
			else
				sub.tex_normal = nullptr;
		}
		else
			sub.tex_normal = nullptr;

		// specular map
		f.ReadString1();
		if(BUF[0])
		{
			sub.tex_specular = res_mgr.LoadTexture(Format("%s/%s", dir.c_str(), BUF));
			f.Read(sub.specular_factor);
			f.Read(sub.specular_color_factor);
		}
		else
			sub.tex_specular = nullptr;

		if(!f)
			throw Format("Failed to read submesh %u.", i);
	}

	// animation data
	if(IS_SET(head.flags, F_ANIMATED) && !IS_SET(head.flags, F_STATIC))
	{
		// bones
		size = Bone::MIN_SIZE * head.n_bones;
		if(!f.Ensure(size))
			throw "Failed to read bones.";
		bones.resize(head.n_bones + 1);

		// zero bone
		Bone& zero_bone = bones[0];
		zero_bone.parent = 0;
		zero_bone.name = "zero";
		zero_bone.id = 0;
		zero_bone.mat.Identity();

		for(byte i = 1; i <= head.n_bones; ++i)
		{
			Bone& bone = bones[i];

			bone.id = i;
			f.Read(bone.parent);

			f.Read(bone.mat._11);
			f.Read(bone.mat._12);
			f.Read(bone.mat._13);
			bone.mat._14 = 0;
			f.Read(bone.mat._21);
			f.Read(bone.mat._22);
			f.Read(bone.mat._23);
			bone.mat._24 = 0;
			f.Read(bone.mat._31);
			f.Read(bone.mat._32);
			f.Read(bone.mat._33);
			bone.mat._34 = 0;
			f.Read(bone.mat._41);
			f.Read(bone.mat._42);
			f.Read(bone.mat._43);
			bone.mat._44 = 1;

			f.Read(bone.name);

			bones[bone.parent].childs.push_back(i);
		}

		if(!f)
			throw "Failed to read bones data.";

		// animations
		size = Animation::MIN_SIZE * head.n_anims;
		if(!f.Ensure(size))
			throw "Failed to read animations.";
		anims.resize(head.n_anims);

		for(byte i = 0; i<head.n_anims; ++i)
		{
			Animation& anim = anims[i];

			f.Read(anim.name);
			f.Read(anim.length);
			f.Read(anim.n_frames);

			size = anim.n_frames * (4 + sizeof(KeyframeBone) * head.n_bones);
			if(!f.Ensure(size))
				throw Format("Failed to read animation %u data.", i);

			anim.frames.resize(anim.n_frames);

			for(word j = 0; j<anim.n_frames; ++j)
			{
				f.Read(anim.frames[j].time);
				anim.frames[j].bones.resize(head.n_bones);
				f.Read(anim.frames[j].bones.data(), sizeof(KeyframeBone) * head.n_bones);
			}
		}

		// add zero bone to count
		++head.n_bones;
	}

	// points
	size = Point::MIN_SIZE * head.n_points;
	if(!f.Ensure(size))
		throw "Failed to read points.";
	attach_points.resize(head.n_points);
	for(word i = 0; i<head.n_points; ++i)
	{
		Point& p = attach_points[i];

		f.Read(p.name);
		f.Read(p.mat);
		f.Read(p.bone);
		f.Read(p.type);
		f.Read(p.size);
		f.Read(p.rot);
		p.rot.y = Clip(-p.rot.y);
	}

	// bone groups
	if(IS_SET(head.flags, F_ANIMATED) && !IS_SET(head.flags, F_STATIC))
	{
		if(!f.Ensure(BoneGroup::MIN_SIZE * head.n_groups))
			throw "Failed to read bone groups.";
		groups.resize(head.n_groups);
		for(word i = 0; i<head.n_groups; ++i)
		{
			BoneGroup& gr = groups[i];

			f.Read(gr.name);

			// parent group
			f.Read(gr.parent);
			assert(gr.parent < head.n_groups);
			assert(gr.parent != i || i == 0);

			// bone indexes
			byte count;
			f.Read(count);
			gr.bones.resize(count);
			f.Read(gr.bones.data(), gr.bones.size());
		}

		if(!f)
			throw "Failed to read bone groups data.";

		SetupBoneMatrices();
	}

	// splits
	if(IS_SET(head.flags, F_SPLIT))
	{
		size = sizeof(Split) * head.n_subs;
		if(!f.Ensure(size))
			throw "Failed to read mesh splits.";
		splits.resize(head.n_subs);
		f.Read(splits.data(), size);
	}
}

void Mesh::SetupBoneMatrices()
{
	model_to_bone.resize(head.n_bones);
	model_to_bone[0].Identity();

	for(word i = 1; i<head.n_bones; ++i)
	{
		const Bone& bone = bones[i];
		bone.mat.Inverse(model_to_bone[i]);

		if(bone.parent > 0)
			model_to_bone[i] = model_to_bone[bone.parent] * model_to_bone[i];
	}
}
