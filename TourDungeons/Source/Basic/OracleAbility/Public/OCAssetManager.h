// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "OCAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class UOCAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	static UOCAssetManager& Get();

	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
};
