#pragma once

#include "Item.h"

struct SceneNode;

struct ItemSlot
{
	Item* item;
	uint count; // stack size or ammo count
};

struct Unit
{
	enum Action
	{
		A_NONE,
		A_RELOAD
	};

	bool CanAct() { return action == A_NONE; }
	bool CanReload();
	uint CountItem(Item* item, bool equipped = false) const;
	Ammo* GetAmmo() const;
	MeleeWeapon& GetMeleeWeapon() { return *(MeleeWeapon*)weapon; }
	RangedWeapon& GetRangedWeapon() { return *(RangedWeapon*)weapon; }
	bool HasMeleeWeapon() const { return weapon && weapon->type == ItemType::MeleeWeapon; }
	bool HasRangedWeapon() const { return weapon && weapon->type == ItemType::RangedWeapon; }
	bool HaveItem(Item* item, bool equipped = false) const;
	void Reload();
	uint RemoveItem(Item* item, uint count = 1, bool equipped = false);
	void Update(float dt);

	SceneNode* node;
	float hp, hpmax;
	uint ammo;
	Action action;
	Item* weapon;
	vector<ItemSlot> items;
};
