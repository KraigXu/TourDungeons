// Copyright 2020 Dan Kestranek.


#include "..\Public\OCAssetManager.h"
#include "AbilitySystemGlobals.h"


UOCAssetManager& UOCAssetManager::Get() 
{
	UOCAssetManager* Singleton = Cast<UOCAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be UOCAssetManager!"));
		return *NewObject<UOCAssetManager>();	 // never calls this
	}
}


void UOCAssetManager::StartInitialLoading() 
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
