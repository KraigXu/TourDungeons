// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OracleAbility : ModuleRules
{
	public OracleAbility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject",
			"Engine", 
			"InputCore",
			"Slate",
			"SlateCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
		
		});
		
	}
}