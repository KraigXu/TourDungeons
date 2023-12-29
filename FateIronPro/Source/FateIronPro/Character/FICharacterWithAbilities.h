// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FICharacter.h"

#include "FICharacterWithAbilities.generated.h"

class UAbilitySystemComponent;
class UFIAbilitySystemComponent;
class UObject;

// ALyraCharacter typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
// alyrachrole通常从拥有玩家状态中获得能力系统组件
//这代表了一个具有自包含能力系统组件的角色。
UCLASS(Blueprintable)
class FATEIRONPRO_API AFICharacterWithAbilities : public AFICharacter
{
	GENERATED_BODY()

public:
	AFICharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "FI|PlayerState")
	TObjectPtr<UFIAbilitySystemComponent> AbilitySystemComponent;
	
	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UFIHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UFICombatSet> CombatSet;
};
