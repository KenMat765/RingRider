// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RingRider : ModuleRules
{
	public RingRider(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "Niagara", "ApexDestruction" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// Necessary to create BehaviorTree Tasks, Decorators, and Services in C++
		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTasks" });

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
