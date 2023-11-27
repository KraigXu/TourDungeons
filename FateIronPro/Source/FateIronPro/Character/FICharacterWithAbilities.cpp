// Copyright Epic Games, Inc. All Rights Reserved.

#include "FICharacterWithAbilities.h"

#include "Async/TaskGraphInterfaces.h"
#include "FateIronPro/AbilitySystem/FIAbilitySystemComponent.h"
#include "FateIronPro/AbilitySystem/Attributes/FICombatSet.h"
#include "FateIronPro/AbilitySystem/Attributes/FIHealthSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FICharacterWithAbilities)

AFICharacterWithAbilities::AFICharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UFIAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UFIHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UFICombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void AFICharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AFICharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

