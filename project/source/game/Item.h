#pragma once

enum class ItemType
{
	MeleeWeapon,
	RangedWeapon,
	Ammo,
	Useable
};

struct Ammo;

struct Item
{
	string id, name;
	ItemType type;
	uint stack_size;

	Item(ItemType type, cstring id, cstring name, uint stack_size) : type(type), id(id), name(name), stack_size(stack_size)
	{

	}

	virtual ~Item() {}
};

struct MeleeWeapon : Item
{
	float dmg;

	MeleeWeapon(cstring id, cstring name, float dmg) : Item(ItemType::MeleeWeapon, id, name, 1), dmg(dmg)
	{

	}
};

struct RangedWeapon : Item
{
	float dmg;
	uint max_ammo;
	string ammo_id;
	Ammo* ammo;

	RangedWeapon(cstring id, cstring name, float dmg, cstring ammo_id, uint max_ammo) : Item(ItemType::RangedWeapon, id, name, 1), dmg(dmg),
		max_ammo(max_ammo), ammo_id(ammo_id), ammo(nullptr)
	{

	}
};

struct Ammo : Item
{
	Ammo(cstring id, cstring name, uint stack_size) : Item(ItemType::Ammo, id, name, stack_size)
	{

	}
};

struct Useable : Item
{
	enum class Effect
	{
		Food,
		Heal
	};
	Effect effect;
	float value;

	Useable(cstring id, cstring name, uint stack_size, Effect effect, float value) : Item(ItemType::Useable, id, name, stack_size), effect(effect),
		value(value)
	{

	}
};

void SetItemPointers();
void CleanupItems();
