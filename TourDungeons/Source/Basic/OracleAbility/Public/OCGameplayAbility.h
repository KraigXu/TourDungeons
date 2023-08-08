// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OracleAbility/OracleAbility.h"
#include "OCGameplayAbility.generated.h"


/**
 *  
 */
UCLASS()
class UOCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UOCGameplayAbility();

	//当输入被按下时，能力将自动激活
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EOCAbilityInputID AbilityInputID = EOCAbilityInputID::None;

	//值将能力与插槽关联，而不将其绑定到自动激活的输入。
	//被动技能不会与输入绑定，所以我们需要一种方法将技能与插槽关联起来。
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EOCAbilityInputID AbilityID = EOCAbilityInputID::None;

	//告诉一个能力被授予时立即激活。用于被动能力和强加于他人的能力。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	//如果一个能力被标记为“activateabilityongranting”，在这里给出时立即激活它们
	// Epic的评论:项目可能想在这里启动被动或做其他“BeginPlay”类型的逻辑。
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
};
