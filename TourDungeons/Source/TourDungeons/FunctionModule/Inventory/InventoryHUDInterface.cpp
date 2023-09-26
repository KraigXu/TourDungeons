// Copyright © 2021-2022 GDI. All Rights Reserved.


#include "InventoryHUDInterface.h"


void IInventoryHUDInterface::UIEquipInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot)
{
}

void IInventoryHUDInterface::UIUnEquipInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot)
{
}

void IInventoryHUDInterface::UIMoveInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot)
{
}

void IInventoryHUDInterface::UISplitInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot, int32 Amount)
{
}

void IInventoryHUDInterface::UISplitItemFromInventory(int32 FromInventorySlot, int32 ToSlot, int32 Amount)
{
}

void IInventoryHUDInterface::UIUseInventoryItem(int32 InventorySlot)
{
}

void IInventoryHUDInterface::UIDropInventoryItem(int32 FromInventorySlot)
{
}

void IInventoryHUDInterface::UIClearHotBarItem(int32 FromInventorySlot)
{
}

void IInventoryHUDInterface::UIEquipFromContainer(int32 FromContainerSlot, int32 ToInventorySlot)
{
}

void IInventoryHUDInterface::UIUnEquipToContainer(int32 FromInventorySlot, int32 ToContainerSlot)
{
}

void IInventoryHUDInterface::UIMoveContainerItem(int32 FromContainerSlot, int32 ToContainerSlot)
{
}

void IInventoryHUDInterface::UISplitContainerItem(int32 FromContainerSlot, int32 ToContainerSlot, int32 Amount)
{
}

void IInventoryHUDInterface::UISplitItemFromContainer(int32 FromContainerSlot, int32 ToSlot, int32 Amount)
{
}

void IInventoryHUDInterface::UIUserContainerItem(int32 ContainerSlot)
{
}

void IInventoryHUDInterface::UITakeContainerItem(int32 FromContainerSlot, int32 ToInventorySlot)
{
}

void IInventoryHUDInterface::UIDepositContainerItem(int32 FromInventorySlot, int32 ToContainerSlot)
{
}

bool IInventoryHUDInterface::UIGetIsShiftKeyDown()
{
    return false;
}

void IInventoryHUDInterface::UIPlaceConstructionItem(TSubclassOf<AActor> ItemClass)
{

}
