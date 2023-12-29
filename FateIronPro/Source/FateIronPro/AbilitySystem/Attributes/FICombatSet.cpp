// Copyright Epic Games, Inc. All Rights Reserved.

#include "FICombatSet.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FICombatSet)

class FLifetimeProperty;

UFICombatSet::UFICombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UFICombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFICombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFICombatSet, DamageType, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFICombatSet, AttackPower, COND_OwnerOnly, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UFICombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UFICombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFICombatSet, BaseDamage, OldValue);
}

void UFICombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFICombatSet, BaseHeal, OldValue);
}

void UFICombatSet::OnRep_DamageType(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFICombatSet,DamageType,OldValue);
}

void UFICombatSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFICombatSet,AttackPower,OldValue);
}

