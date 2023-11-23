// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

FATEIRONPRO_API DECLARE_LOG_CATEGORY_EXTERN(LogFI, Log, All);
FATEIRONPRO_API DECLARE_LOG_CATEGORY_EXTERN(LogFIExperience, Log, All);
FATEIRONPRO_API DECLARE_LOG_CATEGORY_EXTERN(LogFIAbilitySystem, Log, All);
FATEIRONPRO_API DECLARE_LOG_CATEGORY_EXTERN(LogFICombatSystem, Log, All);
FATEIRONPRO_API DECLARE_LOG_CATEGORY_EXTERN(LogFIPawn,Log,All);
FATEIRONPRO_API DECLARE_LOG_CATEGORY_EXTERN(LogFITeams, Log, All);

FATEIRONPRO_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
