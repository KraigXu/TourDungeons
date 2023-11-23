// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIGameData.h"
#include "FIAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIGameData)

UFIGameData::UFIGameData()
{
}

const UFIGameData& UFIGameData::UFIGameData::Get()
{
	return UFIAssetManager::Get().GetGameData();
}
