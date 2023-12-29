// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "FIAttributeSet.h"

#include "FICombatSet.generated.h"

class UObject;
struct FFrame;


/**
 * ULyraCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class UFICombatSet : public UFIAttributeSet
{
	GENERATED_BODY()

public:

	UFICombatSet();

	ATTRIBUTE_ACCESSORS(UFICombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UFICombatSet, DamageType);
	ATTRIBUTE_ACCESSORS(UFICombatSet, BaseHeal);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DamageType(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);
	

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData  AttackPower;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageType, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageType;
	
	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
