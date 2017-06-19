#include "Pch.h"
#include "Core.h"
#include "Color.h"
#include "Mesh.h"
#include "Render.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Texture.h"
#include "DirectXIncl.h"

Scene::Scene() : render(nullptr), clear_color(Color::White), fog_color(0, 0, 0), fog_params(20.f, 40.f, 20.f), light_color(0, 0, 0),
light_dir(VEC3(-1, 2, -1).Normalize()), ambient_color(0.5f, 0.5f, 0.5f)
{
}

Scene::~Scene()
{
	DeleteElements(nodes);
}

void Scene::Add(SceneNode* node)
{
	assert(node);
	nodes.push_back(node);
}

void Scene::Draw()
{
	ListDrawObjects();

	auto device = render->GetDevice();
	auto& mat_viewproj = camera.GetViewProjectionMatrix();
	MATRIX mat_world, mat_combined, m;
	
	// setup shader
	auto e = render->GetShader(Shader::Mesh);
	V(e->SetVector(h_ambient_color, (D3DXVECTOR4*)&ambient_color));
	V(e->SetVector(h_fog_color, (D3DXVECTOR4*)&fog_color));
	V(e->SetVector(h_fog_params, (D3DXVECTOR4*)&fog_params));
	V(e->SetVector(h_light_color, (D3DXVECTOR4*)&light_color));
	V(e->SetVector(h_light_dir, (D3DXVECTOR4*)&light_dir));
	V(e->SetVector(h_camera_pos, (D3DXVECTOR4*)&camera.GetFrom()));

	// draw not-animated meshes
	uint passes;
	V(e->Begin(&passes, 0));
	V(e->BeginPass(0));
	for(auto node : to_draw.normal)
	{
		auto& mesh = *node->mesh;
		mat_world.Rotation(node->rot).Multiply(m.Translation(node->pos));
		mat_combined.Multiply(mat_world, mat_viewproj);

		V(device->SetVertexDeclaration(render->GetVertexDeclaration(mesh.vertex_decl)));
		V(device->SetStreamSource(0, mesh.vb, 0, mesh.vertex_size));
		V(device->SetIndices(mesh.ib));
		V(e->SetMatrix(h_mat_world, (D3DXMATRIX*)&mat_world));
		V(e->SetMatrix(h_mat_combined, (D3DXMATRIX*)&mat_combined));
		V(e->SetVector(h_tint, (D3DXVECTOR4*)&node->tint));

		for(int i = 0; i < mesh.head.n_subs; ++i)
		{
			auto& sub = mesh.subs[i];
			V(e->SetTexture(h_tex_diffuse, sub.tex->tex));
			V(e->SetVector(h_specular_color, (D3DXVECTOR4*)&sub.specular_color));
			V(e->SetFloat(h_specular_intensity, sub.specular_intensity));
			V(e->SetFloat(h_specular_hardness, (float)sub.specular_hardness));
			V(e->CommitChanges());

			V(device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, sub.min_ind, sub.n_ind, sub.first * 3, sub.tris));
		}
	}
	V(e->EndPass());
	V(e->End());
	
	// draw animated meshes
	//	h_mat_bones
}

void Scene::Init(Render* _render)
{
	assert(_render);
	render = _render;
	render->SetScene(this);

	auto e = render->GetShader(Shader::Mesh);
	h_mat_combined = e->GetParameterByName(nullptr, "matCombined");
	h_mat_world = e->GetParameterByName(nullptr, "matWorld");
	h_mat_bones = e->GetParameterByName(nullptr, "matBones");
	h_tint = e->GetParameterByName(nullptr, "tint");
	h_ambient_color = e->GetParameterByName(nullptr, "ambientColor");
	h_fog_color = e->GetParameterByName(nullptr, "fogColor");
	h_fog_params = e->GetParameterByName(nullptr, "fogParams");
	h_light_dir = e->GetParameterByName(nullptr, "lightDir");
	h_light_color = e->GetParameterByName(nullptr, "lightColor");
	h_camera_pos = e->GetParameterByName(nullptr, "cameraPos");
	h_specular_color = e->GetParameterByName(nullptr, "specularColor");
	h_specular_intensity = e->GetParameterByName(nullptr, "specularIntensity");
	h_specular_hardness = e->GetParameterByName(nullptr, "specularHardness");
	h_tex_diffuse = e->GetParameterByName(nullptr, "texDiffuse");
}

void Scene::ListDrawObjects()
{
	to_draw.Clear();
	auto& frustum = camera.GetFrustum();
	for(auto node : nodes)
	{
		if(frustum.SphereToFrustum(node->pos, node->mesh->head.radius))
		{
			if(node->mesh_inst)
				to_draw.animated.push_back(node);
			else
				to_draw.normal.push_back(node);
		}
	}
}

void Scene::Update(float dt)
{
	camera.Update(dt);
}
