// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TourDungeons/Classes/PropItem.h"
#include "InventorySlot.generated.h"

/**
 * 库存的格子缓存
 */
UCLASS()
class UInventorySlot : public UObject
{
    GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventorySlotChangeDelegate, FPropItem, InInventoryItem);

public:
    FPropItem InventoryItem;

    FInventorySlotChangeDelegate SlotChanged;

public:
    UFUNCTION(BlueprintCallable)
    void SetContent(FPropItem Value);

    UFUNCTION(BlueprintCallable)
    bool IsItemValid();
    
};
