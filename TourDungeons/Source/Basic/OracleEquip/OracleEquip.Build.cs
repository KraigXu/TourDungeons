// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OracleEquip : ModuleRules
{
	public OracleEquip(ReadOnlyTargetRules Target) : base(Target)
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
			"OracleAbility"
		});
		
	}
}