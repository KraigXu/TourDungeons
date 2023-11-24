// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIAttributeSet.h"

#include "FateIronPro/AbilitySystem/FIAbilitySystemComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(FIAttributeSet)

class UWorld;


UFIAttributeSet::UFIAttributeSet()
{
}

UWorld* UFIAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UFIAbilitySystemComponent* UFIAttributeSet::GetLyraAbilitySystemComponent() const
{
	return Cast<UFIAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

