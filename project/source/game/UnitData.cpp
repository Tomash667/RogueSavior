#include "Pch.h"
#include "Core.h"
#include "UnitData.h"

std::map<string, UnitData*> g_units = {
	{ "human", new UnitData("human", "Survivor", 100.f, 5.f) },
	{ "zombie", new UnitData("zombie", "Zombie", 100.f, 10.f) },
	{ "zombie_rat", new UnitData("zombie_rat", "Zombie rat", 20.f, 4.f) }
};

void CleanupUnitDatas()
{
	for(auto& u : g_units)
		delete u.second;
}
