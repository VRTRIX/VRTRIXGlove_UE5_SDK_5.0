// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class VRTRIXGlove : ModuleRules
{
	public VRTRIXGlove(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "../VRTRIXGlove/Public"),
                "Runtime/HeadMountedDisplay/Public",
				// ... add public include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "VRTRIXGloveLibrary",
				"Projects",
                "HeadMountedDisplay",
                "RHI",
                "RenderCore",
                "ProceduralMeshComponent",
                // ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
           RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/VRTRIXGloveLibrary/x64/Release/VRTRIXDATAGLOVECLIENT.dll");
        }
	}
}
