// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "FIPawnData.generated.h"

class APawn;
class UFIAbilitySet;
class UFIAbilityTagRelationshipMapping;
class UFICameraMode;
class UFIInputConfig;
class UObject;


/**
 * UFIPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 *	包含用于定义人物的属性不可变数据资产
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "FI Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class FATEIRONPRO_API UFIPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UFIPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FateIron|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FateIron|Abilities")
	TArray<TObjectPtr<UFIAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FateIron|Abilities")
	TObjectPtr<UFIAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	// // Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FateIron|Input")
	TObjectPtr<UFIInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FateIron|Camera")
	TSubclassOf<UFICameraMode> DefaultCameraMode;
};
