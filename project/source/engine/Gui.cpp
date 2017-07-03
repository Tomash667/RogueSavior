#include "Pch.h"
#include "Core.h"
#include "Control.h"
#include "Gui.h"
#include "Render.h"
#include "DirectXIncl.h"

Gui::Gui() : render(nullptr)
{

}

void Gui::Draw()
{
	auto e = render->GetShader(Shader::Gui);
	VEC2 size(render->GetWindowSize());
	uint passes;
	V(e->SetTechnique(h_tech));
	V(e->SetVector(h_size, (D3DXVECTOR4*)&size));
	V(e->Begin(&passes, 0));
	V(e->BeginPass(0));

	Container::Draw();

	V(e->EndPass());
	V(e->End());
}

void Gui::Init(Render* _render)
{
	assert(_render);
	render = _render;
	render->SetGui(this);

	auto e = render->GetShader(Shader::Gui);
	h_tech = e->GetTechniqueByName("gui");
	h_size = e->GetParameterByName(nullptr, "size");
	h_tex = e->GetParameterByName(nullptr, "tex0");
	assert(h_tech && h_size && h_tex);
}
