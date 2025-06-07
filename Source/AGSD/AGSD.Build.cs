// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AGSD : ModuleRules
{
	public AGSD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Niagara", "Slate", "SlateCore", "NavigationSystem" });
	}
}
