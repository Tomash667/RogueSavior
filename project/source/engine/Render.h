#pragma once

#include "ForwardInternal.h"
#include "VertexDeclaration.h"

class Gui;
class Scene;

enum class Shader
{
	Mesh,
	Animated,
	Gui
};

class Render
{
public:
	Render();
	~Render();

	void Init(HWND hwnd, const INT2& window_size, int hz, bool fullscreen);
	void Draw();
	IDirect3DDevice9* GetDevice() { return device; }
	Gui* GetGui() { return gui; }
	Scene* GetScene() { return scene; }
	ID3DXEffect* GetShader(Shader shader);
	IDirect3DVertexDeclaration9* GetVertexDeclaration(VertexDeclarationId declaration_id) { return vertex_decl[declaration_id]; }
	const INT2& GetWindowSize() const { return window_size; }
	void SetGui(Gui* _gui) { gui = _gui; }
	void SetScene(Scene* _scene) { scene = _scene; }

private:
	void CheckCompability();
	void CreateDevice();
	void CreateVertexDeclarations();
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
	IDirect3DVertexDeclaration9* vertex_decl[VDI_MAX];
	Scene* scene;
	Gui* gui;
	HWND hwnd;
	INT2 window_size;
	int hz, shader_version;
	bool fullscreen, lost_device, resources_released;
};
