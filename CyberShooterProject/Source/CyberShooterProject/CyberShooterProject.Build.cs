// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CyberShooterProject : ModuleRules
{
	public CyberShooterProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate","SlateCore", "NavigationSystem" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
