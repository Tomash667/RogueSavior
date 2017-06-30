#include "Pch.h"
#include "Core.h"
#include "GameLoader.h"
#include "Item.h"

// mesh is name.mesh
// tex is name_icon.png
std::map<string, Item*> g_items = {
	{ "baseball_bat", new MeleeWeapon("baseball_bat", "Baseball bat", 10.f) },
	{ "handgun", new RangedWeapon("handgun", "Handgun", 25.f, "handgun_ammo", 10) },
	{ "handgun_ammo", new Ammo("handgun_ammo", "Handgun bullets", 30) },
	{ "canned_food", new Useable("canned_food", "Canned food", 5, Useable::Effect::Food, 50.f) },
	{ "medikit", new Useable("medikit", "Medikit", 1, Useable::Effect::Heal, 25.f) }
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
				weapon->ammo = (Ammo*)ammo;
			else
				GameLoader.AddError("Missing ammo '%s' for ranged weapon '%s'.", weapon->ammo_id.c_str(), weapon->id.c_str());
		}
	}
}

void CleanupItems()
{
	for(auto& i : g_items)
		delete i.second;
}

Item* GetItem(cstring id)
{
	assert(id);
	auto item = FindItem(id);
	if(!item)
		throw Format("Missing item '%s'.", id);
	return item;
}