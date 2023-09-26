// Copyright © 2021-2022 GDI. All Rights Reserved.


#include "EquipmentInventoryComponent.h"

UEquipmentInventoryComponent::UEquipmentInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

bool UEquipmentInventoryComponent::InitInventory(int32 InInventorySize)
{
    Inventory.Reset(0);

    for (int i = 0; i < InventorySlotMax; i++)
    {
        Inventory.Add(FPropItem());
    }
    return true;
}

void UEquipmentInventoryComponent::GetEmptyInventorySpace(bool& Success, int32& Index)
{
    int LocalIndex = -1;
    for (int i = EquipmentSlots; i < Inventory.Num(); i++)
    {
        if (Inventory[i].IsValid() == false)
        {
            LocalIndex = i;
            Success = true;
            break;
        }
    }
    Index = LocalIndex;
}

int32 UEquipmentInventoryComponent::GetInventoryItemCount() const
{
    int32 LocalItemCount = 0;
    for (int i = EquipmentSlots; i < Inventory.Num(); i++)
    {
        if (Inventory[i].IsValid() == true)
        {
            LocalItemCount++;
        }
    }
    return LocalItemCount;
}

bool UEquipmentInventoryComponent::SetInventoryItem(int32 InInventorySlot, FPropItem InInventoryItem)
{
    if(Super::SetInventoryItem(InInventorySlot, InInventoryItem))
    {
        bool bFlag = Super::SetInventoryItem(InInventorySlot, InInventoryItem);
        //TODO:更新物体参数

        //TODO:更新物体属性
        
        UpdateEquippedMeshes(InInventorySlot);

        return true;
    }else
    {
        return false;
    }
}

void UEquipmentInventoryComponent::UpdateEquippedMeshes(int32 InventorySlot)
{
    // if (PawnRef != nullptr)
    // {
    //     if (InventorySlot < EquipmentSlots)
    //     {
    //         FPropItem LocalInventoryItem = GetInventoryItem(InventorySlot);
    //         UStaticMesh* LocalEquipmentMesh = LocalInventoryItem.EquipmentMesh;
    //
    //         //这里针对不同的槽更新装备状态
    //         if (InventorySlot == 0)
    //         {
    //             //更新头部装备
    //             PawnRef->HeadMesh = LocalEquipmentMesh;
    //         }
    //         else if (InventorySlot == 1)
    //         {
    //             //更新身体装备
    //             PawnRef->BodyMesh = LocalEquipmentMesh;
    //         }
    //         else if (InventorySlot == 2)
    //         {
    //             //更新手部装备
    //             PawnRef->HandMesh = LocalEquipmentMesh;
    //         }
    //         else if (InventorySlot == 3)
    //         {
    //             //更新脚部装备
    //             PawnRef->FootMesh = LocalEquipmentMesh;
    //         }
    //         else if (InventorySlot == 4)
    //         {
    //             //更新手持装备
    //             UpdateMainHandTool(LocalEquipmentMesh, LocalInventoryItem.ID);
    //         }
    //         else
    //         {
    //             //逻辑不应走到这里
    //         }
    //     }
    // }
}

void UEquipmentInventoryComponent::UpdateMainHandTool(UStaticMesh* InEquipmentMesh, int32 InToolId)
{
    // UStaticMesh* LocalEquipmentMesh = InEquipmentMesh;
    // APlayerController* LocalPlayerController = Cast<APlayerController>(GetOwner());
    // if (APickableBase* LocalPickable = APickableBase::GetPickableById(LocalPlayerController, InToolId))
    // {
    //     FAttachmentTransformRules LocalRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
    //     LocalPickable->AttachToComponent(PawnRef->GetMesh(), LocalRules, MainHandSocket);
    // }
    // else
    // {
    //     //更新设备网格
    //     PawnRef->MainHandMesh = InEquipmentMesh;
    // }
}
