// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;

public class LagCompensation : ModuleRules
{
	public LagCompensation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		var PublicPaths = ModuleDirectory + "/Public";
		var PrivatePaths = ModuleDirectory + "/Private";
		
		PublicIncludePaths.AddRange(
			new string[]
			{
				PublicPaths,
				PublicPaths + "/Asyncs",
				PublicPaths + "/Components",
				PublicPaths + "/Data",
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				PrivatePaths,
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"PhysicsCore",
			}
		);
		
		SetupModulePhysicsSupport(Target);
		
		/*if (Target.bCompilePhysX && (Target.bBuildEditor || Target.bCompileAPEX))
		{
			DynamicallyLoadedModuleNames.Add("PhysXCooking");
		}*/

		/*
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"PhysicsSQ",
				"ChaosSolvers"
			}
		);*/


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}