// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DressAppearanceComponent.generated.h"

/**
 *背包系统  管控使用者当前所有物的组件
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UDressAppearanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDressAppearanceComponent();
	
 
};
