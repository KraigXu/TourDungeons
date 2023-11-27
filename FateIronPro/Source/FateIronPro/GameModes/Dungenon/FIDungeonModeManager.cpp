// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIDungeonModeManager.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIDungeonModeManager)

#if WITH_EDITOR

void UFIDungeonModeManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UFIDungeonModeManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UFIDungeonModeManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UFIDungeonModeManager>();
		check(ExperienceManagerSubsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool UFIDungeonModeManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		UFIDungeonModeManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UFIDungeonModeManager>();
		check(ExperienceManagerSubsystem);

		// Only let the last requester to get this far deactivate the plugin
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}

	return true;
}

#endif
