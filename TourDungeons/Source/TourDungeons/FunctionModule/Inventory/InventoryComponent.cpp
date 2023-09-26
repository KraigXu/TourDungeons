// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::ServerInitInventory_Implementation(int32 InInventorySize)
{
    InitInventory(InInventorySize);
}

bool UInventoryComponent::InitInventory(int32 InInventorySize)
{
    Inventory.Reset(0);

    for (int i = 0; i < InInventorySize; i++)
    {
        Inventory.Add(FPropItem());
    }
    if (Inventory.Num() == InInventorySize)
    {
        //
        return true;
    }
    else
    {
        //
        return false;
    }
}


TArray<FPropItem> UInventoryComponent::GetInventoryItems()
{
    return Inventory;
}

FPropItem UInventoryComponent::GetInventoryItem(int InInventorySlot)
{
    return Inventory[InInventorySlot];
}

bool UInventoryComponent::SetInventoryItem(int32 InInventorySlot, FPropItem InventoryItem)
{
    Inventory[InInventorySlot] = InventoryItem;
    OnInventorySetItem.Broadcast(InInventorySlot,InventoryItem);
    return true;
}

bool UInventoryComponent::ClearInventoryItem(int32 InInventorySlot)
{
    Inventory[InInventorySlot] = FPropItem();
    OnInventoryClearItem.Broadcast(InInventorySlot);
    return true;
}

void UInventoryComponent::GetEmptyInventorySpace(bool& Success, int32& Index)
{
    for (int i = 0; i < Inventory.Num(); i++)
    {
        if (Inventory[i].IsValid() == false)
        {
            Index = i;
            Success = true;
        }
    }
    Success = false;
    Index = 0;
}

int32 UInventoryComponent::GetInventoryItemCount() const
{
    int32 LocalItemCount = 0;
    for (int i = 0; i < Inventory.Num(); i++)
    {
        if (Inventory[i].IsValid() == true)
            LocalItemCount++;
    }
    return LocalItemCount;
}

int32 UInventoryComponent::GetInventoryItemCountByClass(TSubclassOf<AActor> InClass, const bool bExactlyEqual) const
{
    return Inventory.FilterByPredicate([InClass, bExactlyEqual](const FPropItem X) { return X.IsValid() && (bExactlyEqual ? X.ItemClass == InClass : X.ItemClass && X.ItemClass.GetDefaultObject()->IsA(InClass)); }).Num();
}

void UInventoryComponent::IncreaseInventorySize(int32 InAmount)
{
}

void UInventoryComponent::DecreaseInventorySize(int32 InAmount)
{
}

void UInventoryComponent::LoadInventoryItems(int32 InInventorySize, TArray<FPropItem> InInventoryItems)
{
    Inventory.Empty();

    for (int i = 0; i < InInventorySize; i++)
    {
        Inventory[i] = FPropItem();
    }

    for (int i = 0; i < InInventoryItems.Num(); i++)
    {
        SetInventoryItem(i, InInventoryItems[i]);
    }
}

FPropItem UInventoryComponent::AddToItemAmount(FPropItem InventoryItem, int32 AmountToAdd)
{
    InventoryItem.Amount += AmountToAdd;
    return InventoryItem;
}

void UInventoryComponent::SendNumberChangeEvent()
{
    if (OnInventoryNumberUpdate.IsBound())
    {
        OnInventoryNumberUpdate.Broadcast(GetInventoryItemCount(), UpperLimit);
    }
}
