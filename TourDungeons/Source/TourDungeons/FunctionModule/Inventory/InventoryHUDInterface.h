// Copyright © 2021-2022 GDI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryHUDInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInventoryHUDInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 库存HUD 界面接口
 * 用来做UI交互相关
 */
class IInventoryHUDInterface
{
    GENERATED_BODY()
public:
    virtual void UIEquipInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot);

    virtual void UIUnEquipInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot);

    virtual void UIMoveInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot);

    virtual void UISplitInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot, int32 Amount);

    virtual void UISplitItemFromInventory(int32 FromInventorySlot, int32 ToSlot, int32 Amount);

    virtual void UIUseInventoryItem(int32 InventorySlot);

    virtual void UIDropInventoryItem(int32 FromInventorySlot);

    virtual void UIClearHotBarItem(int32 FromInventorySlot);

    virtual void UIEquipFromContainer(int32 FromContainerSlot, int32 ToInventorySlot);

    virtual void UIUnEquipToContainer(int32 FromInventorySlot, int32 ToContainerSlot);

    virtual void UIMoveContainerItem(int32 FromContainerSlot, int32 ToContainerSlot);

    virtual void UISplitContainerItem(int32 FromContainerSlot, int32 ToContainerSlot, int32 Amount);

    virtual void UISplitItemFromContainer(int32 FromContainerSlot, int32 ToSlot, int32 Amount);

    virtual void UIUserContainerItem(int32 ContainerSlot);

    virtual void UITakeContainerItem(int32 FromContainerSlot, int32 ToInventorySlot);

    virtual void UIDepositContainerItem(int32 FromInventorySlot, int32 ToContainerSlot);

    virtual bool UIGetIsShiftKeyDown();

    virtual void UIPlaceConstructionItem(TSubclassOf<AActor> ItemClass);
};
