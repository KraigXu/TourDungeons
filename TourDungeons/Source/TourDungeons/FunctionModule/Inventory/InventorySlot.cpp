// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"

void UInventorySlot::SetContent(FPropItem Value)
{
	InventoryItem = Value;

	if (SlotChanged.IsBound())
	{
	    UE_LOG(LogTemp, Log, TEXT("提示：背包储存发生变动"));
		SlotChanged.Broadcast(Value);
	}
}

bool UInventorySlot::IsItemValid()
{
    if(InventoryItem.ID==-1)
    {
        return false;
    }
    return true;
}
