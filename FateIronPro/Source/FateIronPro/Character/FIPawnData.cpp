// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIPawnData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIPawnData)

UFIPawnData::UFIPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}

