#include "PropItem.h"

int32 FPropItem::ItemNextId = 8999999;

bool FPropItem::IsValid() const
{
	if (ID == -1)
	{
		return false;
	}
	return true;
}

int32 FPropItem::ItemFreshStackSpace()
{
	return MaxStackSize - Amount;
}

bool FPropItem::ItemIsSame(FPropItem ToItem)
{
	if (ToItem.Icon == Icon)
	{
		return true;
	}
	return false;
}

bool FPropItem::IsDroppable()
{
	return bIsDroppable;
}

void FPropItem::NextItemId()
{
	ItemNextId++;
	ID = ItemNextId;
}

int32 FPropItem::GetEquipmentSlotsIndex(EEquipmentSlots InType)
{
	int32 LocalSlotIndex = -1;
	switch (InType)
	{
	case EEquipmentSlots::Head:
		LocalSlotIndex = 0;
		break;
	case EEquipmentSlots::Chest:
		LocalSlotIndex = 1;
		break;
	case EEquipmentSlots::Hands:
		LocalSlotIndex = 2;
		break;
	case EEquipmentSlots::Feet:
		LocalSlotIndex = 3;
		break;
	case EEquipmentSlots::MainHand:
		LocalSlotIndex = 4;
		break;
	case EEquipmentSlots::OffHand:
		LocalSlotIndex = 4;
		break;
	default: ;
	}
	return LocalSlotIndex;
}
