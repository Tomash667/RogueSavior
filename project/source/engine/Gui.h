#pragma once

class Control;
class Render;

class Gui
{
public:
	Gui();
	~Gui();

	void Add(Control* control);
	void Draw();
	Render* GetRender() { return render; }
	void Init(Render* render);

private:
	Render* render;
	vector<Control*> controls;
};
