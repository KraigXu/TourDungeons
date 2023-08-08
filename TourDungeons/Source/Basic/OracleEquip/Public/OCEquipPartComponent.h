// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "OCEquipPartComponent.generated.h"

/**
 * 装备部位的类型
 *
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ORACLEEQUIP_API UOCEquipPartComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOCEquipPartComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * 部位是否有效
	 * @brief 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	bool IsPartValid();
};
