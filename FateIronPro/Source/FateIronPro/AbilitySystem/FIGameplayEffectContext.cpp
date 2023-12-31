// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIGameplayEffectContext.h"

#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FIAbilitySourceInterface.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIGameplayEffectContext)

class FArchive;

FFIGameplayEffectContext* FFIGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FFIGameplayEffectContext::StaticStruct()))
	{
		return (FFIGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FFIGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

// #if UE_WITH_IRIS
// namespace UE::Net
// {
// 	// Forward to FGameplayEffectContextNetSerializer
// 	// Note: If FLyraGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
// 	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(FIGameplayEffectContext, FGameplayEffectContextNetSerializer);
// }
// #endif

void FFIGameplayEffectContext::SetAbilitySource(const IFIAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IFIAbilitySourceInterface* FFIGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IFIAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FFIGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

