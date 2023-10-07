// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GrowablePawnData.generated.h"

class APawn;
class UOCAbilitySet;
class UObject;

/**
 * UGrowablePawnData
 *  
 */
UCLASS(BlueprintType,Const,Meta = (DisplayName = "Growable Pawn Data", ShortTooltip = "Data asset used to Growable a Pawn."))
class TOURDUNGEONS_API UGrowablePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UGrowablePawnData(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UOCAbilitySet>> AbilitySets;
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	// TObjectPtr<ULyraAbilityTagRelationshipMapping> TagRelationshipMapping;
	//
	// // Input configuration used by player controlled pawns to create input mappings and bind input actions.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Input")
	// TObjectPtr<ULyraInputConfig> InputConfig;
	//
	// // Default camera mode used by player controlled pawns.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	// TSubclassOf<ULyraCameraMode> DefaultCameraMode;
	//
};
