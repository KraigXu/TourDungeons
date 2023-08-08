// Copyright © 2021-2022 GDI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OCEquipPartComponent.h"
#include "Components/ActorComponent.h"
#include "OCEquipManager.generated.h"

/**
 * 装备组件
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UOCEquipManager : public UActorComponent
{
    GENERATED_BODY()
public:
	UOCEquipManager();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UOCEquipPartComponent*> EquipPartComponents;
	
protected:
	
	virtual void BeginPlay() override;
};

