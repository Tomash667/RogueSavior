#include "Pch.h"
#include "Core.h"
#include "Control.h"
#include "Gui.h"
#include "Render.h"

Gui::Gui()
{

}

Gui::~Gui()
{
	DeleteElements(controls);
}

void Gui::Add(Control* control)
{
	assert(control);
	controls.push_back(control);
}

void Gui::Draw()
{
	for(auto control : controls)
		control->Draw();
}

void Gui::Init(Render* _render)
{
	assert(_render);
	render = _render;
	render->SetGui(this);
}
