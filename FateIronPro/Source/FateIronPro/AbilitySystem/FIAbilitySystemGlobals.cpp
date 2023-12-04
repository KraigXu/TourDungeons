// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIAbilitySystemGlobals.h"

#include "FIGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIAbilitySystemGlobals)

struct FGameplayEffectContext;

UFIAbilitySystemGlobals::UFIAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UFIAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FFIGameplayEffectContext();
}

