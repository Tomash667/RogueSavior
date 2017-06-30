#pragma once

struct UnitData
{
	string id, name;
	float hp, dmg;

	UnitData(cstring id, cstring name, float hp, float dmg) : id(id), name(name), hp(hp), dmg(dmg)
	{

	}
};

void CleanupUnitDatas();
