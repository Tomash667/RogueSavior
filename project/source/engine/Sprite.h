#pragma once

#include "Control.h"
#include "Texture.h"

class Sprite : public Control
{
public:
	Sprite(Texture* tex);

	void Draw() override;

private:
	Texture* tex;
};
