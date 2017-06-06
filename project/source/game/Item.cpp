#include "Pch.h"
#include "Core.h"
#include "Item.h"

std::map<string, Item*> g_items = {
	{ "baseball_bat", new MeleeWeapon("baseball_bat", "Baseball bat", 10.f) },
	{ "handgun", new RangedWeapon("handgun", "Handgun", 25.f, "handgun_ammo", 10) },
	{ "handgun_ammo", new Ammo("handgun_ammo", "Handgun bullets", 30) },
	{ "food_can", new Useable("food_can", "Food can", 5, Useable::Effect::Food, 50.f) },
	{ "first_aid", new Useable("first_aid", "First aid kit", 1, Useable::Effect::Heal, 25.f) }
};

Item* FindItem(const string& id)
{
	auto it = g_items.find(id);
	if(it == g_items.end())
		return nullptr;
	else
		return it->second;
}

void SetItemPointers()
{
	for(auto& i : g_items)
	{
		auto item = i.second;
		if(item->type == ItemType::RangedWeapon)
		{
			auto weapon = (RangedWeapon*)item;
			auto ammo = FindItem(weapon->ammo_id);
			if(ammo && ammo->type == ItemType::Ammo)
				Error("Missing ammo '%s' for ranged weapon '%s'.", weapon->id.c_str(), weapon->ammo_id.c_str());
			else
				weapon->ammo = (Ammo*)ammo;
		}
	}
}
