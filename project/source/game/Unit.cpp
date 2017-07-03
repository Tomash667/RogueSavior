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

Ammo* Unit::GetAmmo() const
{
	if(weapon && weapon->type == ItemType::RangedWeapon)
		return ((RangedWeapon*)weapon)->ammo;
	else
		return nullptr;
}

uint Unit::CountItem(Item* item, bool equipped) const
{
	uint count = 0u;
	if(equipped)
	{
		if(weapon == item)
			++count;
	}
	if(item->stack_size == 1u)
	{
		for(auto& slot : items)
		{
			if(slot.item == item)
				++count;
		}
	}
	else
	{
		for(auto& slot : items)
		{
			if(slot.item == item)
				count += slot.count;
		}
	}
	return count;
}

bool Unit::HaveItem(Item* item, bool equipped) const
{
	assert(item);
	if(equipped)
	{
		if(item == weapon)
			return true;
	}
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

uint Unit::RemoveItem(Item* item, uint count, bool equipped)
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
		for(auto& slot : items)
		{
			if(slot.item == item)
			{
				uint to_remove = max(slot.count, count - removed);
				slot.count -= to_remove;
				if(slot.count == 0u)
					slot.item = nullptr;
				removed += to_remove;
				if(removed == count)
					return removed;
			}
		}
	}
	if(equipped)
	{
		if(weapon == item)
		{
			++removed;
			weapon = nullptr;
			ammo = 0u;
			if(removed == count)
				return removed;
		}
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
