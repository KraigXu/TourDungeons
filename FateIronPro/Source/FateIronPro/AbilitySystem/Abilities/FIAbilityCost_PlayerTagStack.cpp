// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIAbilityCost_PlayerTagStack.h"

#include "FIGameplayAbility.h"
#include "GameFramework/Controller.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIAbilityCost_PlayerTagStack)

UFIAbilityCost_PlayerTagStack::UFIAbilityCost_PlayerTagStack()
{
	Quantity.SetValue(1.0f);
}

bool UFIAbilityCost_PlayerTagStack::CheckCost(const UFIGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		//if (ALyraPlayerState* PS = Cast<ALyraPlayerState>(PC->PlayerState))
		//{
		//	const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
//
		//	const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
		//	const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
//
		//	return PS->GetStatTagStackCount(Tag) >= NumStacks;
		//}
	}
	return false;
}

void UFIAbilityCost_PlayerTagStack::ApplyCost(const UFIGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			//if (ALyraPlayerState* PS = Cast<ALyraPlayerState>(PC->PlayerState))
			//{
			//	const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
//
			//	const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			//	const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
//
			//	PS->RemoveStatTagStack(Tag, NumStacks);
			//}
		}
	}
}

