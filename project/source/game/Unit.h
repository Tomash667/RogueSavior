#pragma once

#include "Item.h"

struct ItemSlot
{
	Item* item;
	uint count;
};

struct Unit
{
	int hp, hpmax, str, end, dex, Int, cha;

	MeleeWeapon* melee_weapon;
	RangedWeapon* ranged_weapon;
	vector<ItemSlot> items;
};
