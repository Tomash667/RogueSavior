#pragma once

#include "Control.h"

class Container : public Control
{
public:
	~Container();

	void Draw() override;
	void Update(float dt) override;

	void Add(Control* control);

protected:
	vector<Control*> controls;
};
