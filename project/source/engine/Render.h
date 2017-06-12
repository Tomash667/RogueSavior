#pragma once

#include "ForwardInternal.h"

class Scene;

enum class Shader
{
	Mesh,
	Animated
};

class Render
{
public:
	Render();
	~Render();

	void Init(HWND hwnd, const INT2& window_size, int hz, bool fullscreen);
	void Draw();
	IDirect3DDevice9* GetDevice() { return device; }
	Scene* GetScene() { return scene; }
	ID3DXEffect* GetShader(Shader shader);
	void SetScene(Scene* _scene) { scene = _scene; }

private:
	void CheckCompability();
	void CreateDevice();
	void GatherParams(D3DPRESENT_PARAMETERS& d3dpp);
	void LoadShaders();
	ID3DXEffect* LoadShader(cstring name, ID3DXEffectPool* pool, cstring param = nullptr);
	void OnLostDevice();
	void OnResetDevice();
	bool Reset();

	IDirect3D9* d3d;
	IDirect3DDevice9* device;
	ID3DXEffectPool* effect_pool;
	ID3DXEffect* e_mesh, *e_animated, *e_gui;
	Scene* scene;
	HWND hwnd;
	INT2 window_size;
	int hz, shader_version;
	bool fullscreen, lost_device, resources_released;
};
