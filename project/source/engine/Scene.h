#pragma once

#include "Camera.h"
#include "ForwardInternal.h"

struct SceneNode;
class Render;

class Scene
{
public:
	Scene();
	~Scene();

	void Add(SceneNode* node);
	void Draw();
	void Init(Render* render);
	const VEC3& GetAmbientColor() { return ambient_color; }
	Camera& GetCamera() { return camera; }
	uint GetClearColor() { return clear_color; }
	const VEC3& GetFogColor() { return fog_color; }
	const VEC3& GetFogParams() { return fog_params; }
	const VEC3& GetLightColor() { return light_color; }
	const VEC3& GetLightDir() { return light_dir; }
	void SetClearColor(uint _clear_color) { clear_color = _clear_color; }
	void SetFogColor(const VEC3& _fog_color) { fog_color = _fog_color; }
	void SetFogParams(const VEC3& _fog_params) { fog_params = _fog_params; }
	void SetFogParams(float start, float end) { fog_params = VEC3(start, end, end - start); }
	void SetLightColor(const VEC3& _light_color) { light_color = _light_color; }
	void SetLightDir(const VEC3& _light_dir) { light_dir = _light_dir; }

private:
	struct DrawBatch
	{
		vector<SceneNode*> normal;
		vector<SceneNode*> animated;

		void Clear()
		{
			normal.clear();
			animated.clear();
		}
	};
	void ListDrawObjects();

	Render* render;
	vector<SceneNode*> nodes;
	DrawBatch to_draw;
	Camera camera;
	uint clear_color;
	D3DXHANDLE h_mat_combined, h_mat_world, h_mat_bones, h_tint, h_ambient_color, h_fog_color, h_fog_params, h_light_dir, h_light_color, h_camera_pos, h_specular_color,
		h_specular_intensity, h_specular_hardness, h_tex_diffuse;
	VEC3 fog_params; // fog start, fog end, fog end - fog start
	VEC3 fog_color, light_color, ambient_color;
	VEC3 light_dir; // vector from (0,0,0) to sun (x,y,z) - normalized
};
