#pragma once

#include "Container.h"
#include "ForwardInternal.h"

class Render;

class Gui : public Container
{
public:
	Gui();

	void Draw() override;
	Render* GetRender() { return render; }
	void Init(Render* render);

private:
	Render* render;
	D3DXHANDLE h_tech, h_size, h_tex;
};
