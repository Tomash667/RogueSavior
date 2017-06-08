#pragma once

struct UnitData
{
	string id, name;

	UnitData(cstring id, cstring name) : id(id), name(name)
	{

	}
};

void CleanupUnitDatas();
