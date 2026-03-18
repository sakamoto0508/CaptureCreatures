// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CaptureCreatures : ModuleRules
{
	public CaptureCreatures(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"CaptureCreatures",
			"CaptureCreatures/Variant_Platforming",
			"CaptureCreatures/Variant_Platforming/Animation",
			"CaptureCreatures/Variant_Combat",
			"CaptureCreatures/Variant_Combat/AI",
			"CaptureCreatures/Variant_Combat/Animation",
			"CaptureCreatures/Variant_Combat/Gameplay",
			"CaptureCreatures/Variant_Combat/Interfaces",
			"CaptureCreatures/Variant_Combat/UI",
			"CaptureCreatures/Variant_SideScrolling",
			"CaptureCreatures/Variant_SideScrolling/AI",
			"CaptureCreatures/Variant_SideScrolling/Gameplay",
			"CaptureCreatures/Variant_SideScrolling/Interfaces",
			"CaptureCreatures/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
