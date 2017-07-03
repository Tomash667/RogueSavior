#pragma once

#include "Item.h"

struct SceneNode;

struct GroundItem
{
	SceneNode* node;
	Item* item;
	uint ammo_count;
};
