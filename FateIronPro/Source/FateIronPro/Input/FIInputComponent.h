// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EnhancedInputComponent.h"
#include "FIInputConfig.h"

#include "FIInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;


/**
 * UFIInputComponent
 *
 *	Component used to manage input mappings and bindings using an input config data asset.
 */
UCLASS(Config = Input)
class UFIInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UFIInputComponent(const FObjectInitializer& ObjectInitializer);

	// void AddInputMappings(const UFIInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	// void RemoveInputMappings(const UFIInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	//
	// template<class UserClass, typename FuncType>
	// void BindNativeAction(const UFIInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);
	//
	// template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	// void BindAbilityActions(const UFIInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
	//
	// void RemoveBinds(TArray<uint32>& BindHandles);
};

//
// template<class UserClass, typename FuncType>
// void UFIInputComponent::BindNativeAction(const UFIInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
// {
// 	check(InputConfig);
// 	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
// 	{
// 		BindAction(IA, TriggerEvent, Object, Func);
// 	}
// }
//
// template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
// void UFIInputComponent::BindAbilityActions(const UFIInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
// {
// 	check(InputConfig);
//
// 	for (const FFIInputAction& Action : InputConfig->AbilityInputActions)
// 	{
// 		if (Action.InputAction && Action.InputTag.IsValid())
// 		{
// 			if (PressedFunc)
// 			{
// 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
// 			}
//
// 			if (ReleasedFunc)
// 			{
// 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
// 			}
// 		}
// 	}
// }
