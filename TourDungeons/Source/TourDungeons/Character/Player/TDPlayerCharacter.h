﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OCCharacterBase.h"
#include "TDPlayerCharacter.generated.h"

UCLASS()
class TOURDUNGEONS_API ATDPlayerCharacter : public AOCCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
