#include "Pch.h"
#include "Core.h"
#include "Container.h"

Container::~Container()
{
	DeleteElements(controls);
}

void Container::Add(Control* control)
{
	assert(control);
	controls.push_back(control);
	control->parent = this;
}

void Container::Draw()
{
	for(auto control : controls)
		control->Draw();
}

void Container::Update(float dt)
{
	for(auto control : controls)
		control->Update(dt);
}
