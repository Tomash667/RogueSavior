#include "Pch.h"
#include "Core.h"
#include "UnitData.h"

std::map<string, UnitData*> g_units = {
	{ "human", new UnitData("human", "Survivor") },
	{ "zombie", new UnitData("zombie", "Zombie") },
	{ "zombie_rat", new UnitData("zombie_rat", "Zombie rat") }
};

void CleanupUnitDatas()
{
	for(auto& u : g_units)
		delete u.second;
}
