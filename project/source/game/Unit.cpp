#include "Pch.h"
#include "Core.h"
#include "Unit.h"

bool Unit::CanReload()
{
	if(!HasRangedWeapon())
		return false;

	auto& wep = GetRangedWeapon();
	if(wep.max_ammo == ammo)
		return false;

	return HaveItem(wep.ammo);
}

bool Unit::HaveItem(Item* item) const
{
	assert(item);
	for(auto& slot : items)
	{
		if(slot.item == item)
			return true;
	}
	return false;
}

void Unit::Reload()
{
	action = A_RELOAD;
	// start animation
}

uint Unit::RemoveItem(Item* item, uint count)
{
	assert(item && count > 0u);
	uint removed = 0u;
	if(item->stack_size == 1u)
	{
		for(auto& slot : items)
		{
			if(slot.item == item)
			{
				slot.item = nullptr;
				++removed;
				if(removed == count)
					return removed;
			}
		}
	}
	else
	{
		// TODO
		/*for(auto& slot : items)
		{
			if(slot.item == item)
			{
				slot.count -= ma
				slot.item = nullptr;
				++removed;
				if(removed == count)
					return removed;
			}
		}*/
	}
	return removed;
}

void Unit::Update(float dt)
{
	switch(action)
	{
	case A_RELOAD:
		// at correct point
		if(HasRangedWeapon())
		{
			auto& wep = GetRangedWeapon();
			uint missing_ammo = wep.max_ammo - ammo;
			if(missing_ammo > 0)
			{
				uint removed = RemoveItem(wep.ammo, missing_ammo);
				ammo += removed;
			}
		}
		break;
	}
}
