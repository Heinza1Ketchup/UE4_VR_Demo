// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE4_VR_Demo : ModuleRules
{
	public UE4_VR_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "HeadMountedDisplay", "CableComponent" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        //PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
       //PrivateIncludePathModuleNames.AddRange(new string[] { "CableComponent" });

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
