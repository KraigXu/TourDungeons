// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIInputComponent.h"

#include "EnhancedInputSubsystems.h"
// #include "Player/LyraLocalPlayer.h"
// #include "Settings/LyraSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIInputComponent)

class UFIInputConfig;

UFIInputComponent::UFIInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UFIInputComponent::AddInputMappings(const UFIInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
 {
 	check(InputConfig);
 	check(InputSubsystem);

 	// Here you can handle any custom logic to add something from your input config if required
 }

void UFIInputComponent::RemoveInputMappings(const UFIInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
 {
 	check(InputConfig);
 	check(InputSubsystem);

 	// Here you can handle any custom logic to remove input mappings that you may have added above
 }

 void UFIInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
 {
 	for (uint32 Handle : BindHandles)
 	{
 		RemoveBindingByHandle(Handle);
 	}
 	BindHandles.Reset();
 }
