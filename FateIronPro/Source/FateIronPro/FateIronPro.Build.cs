// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FateIronPro : ModuleRules
{
	public FateIronPro(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.AddRange(
			new string[] {
				"FateIronPro"
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"CoreUObject",
				"Engine", 
				"PhysicsCore",
				"InputCore",
				"GameplayTags",
				"GameplayTasks",
				"GameplayAbilities",
				"AIModule",
				"ModularGameplay",
				"ModularGameplayActors",
				"CommonLoadingScreen",
				"GameFeatures"
			});

		PrivateDependencyModuleNames.AddRange(
            new string[]
            {
	            "InputCore",
	            "Slate",
	            "SlateCore",
	            "RenderCore",
	            "NetCore",
	            "UMG",
	            "CommonUI",
	            "CommonInput",
	            "CommonGame",
	            "CommonUser",
	            "EnhancedInput"
            });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
