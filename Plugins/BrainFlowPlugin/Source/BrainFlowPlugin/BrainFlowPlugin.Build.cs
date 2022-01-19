// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class BrainFlowPlugin : ModuleRules
{
	public BrainFlowPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        if(Target.Platform == UnrealTargetPlatform.Win32)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "inc"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "Brainflow32.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "DataHandler32.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "BoardController32.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "MLModule32.lib"));
			
			/* RuntimeDependencies.Add("$(TargetOutputDir)/BoardController32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "BoardController32.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/DataHandler32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "DataHandler32.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/GanglionLib32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "GanglionLib32.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/neurosdk-x86.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "neurosdk-x86.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/BrainBitLib32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "BrainBitLib32.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/eego-SDK32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "eego-SDK32.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/BrainFlowBluetooth32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "BrainFlowBluetooth32.dll")); */
            
            RuntimeDependencies.Add("$(BinaryOutputDir)/BoardController32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "BoardController32.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/DataHandler32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "DataHandler32.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/GanglionLib32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "GanglionLib32.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/neurosdk-x86.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "neurosdk-x86.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/BrainBitLib32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "BrainBitLib32.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/eego-SDK32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "eego-SDK32.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/BrainFlowBluetooth32.dll", Path.Combine(ModuleDirectory, "Compiled", "Win32_dynamic", "lib", "BrainFlowBluetooth32.dll"));
        }
        if(Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "inc"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "Brainflow.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "DataHandler.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "BoardController.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "MLModule.lib"));
			
            /* RuntimeDependencies.Add("$(TargetOutputDir)/BoardController.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "BoardController.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/DataHandler.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "DataHandler.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/GanglionLib.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "GanglionLib.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/neurosdk-x64.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "neurosdk-x64.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/Unicorn.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "Unicorn.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/BrainBitLib.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "BrainBitLib.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/eego-SDK.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "eego-SDK.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/BrainFlowBluetooth.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "BrainFlowBluetooth.dll")); */

            RuntimeDependencies.Add("$(BinaryOutputDir)/BoardController.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "BoardController.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/DataHandler.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "DataHandler.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/GanglionLib.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "GanglionLib.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/neurosdk-x64.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "neurosdk-x64.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/Unicorn.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "Unicorn.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/BrainBitLib.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "BrainBitLib.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/eego-SDK.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "eego-SDK.dll"));
            RuntimeDependencies.Add("$(BinaryOutputDir)/BrainFlowBluetooth.dll", Path.Combine(ModuleDirectory, "Compiled", "x64_dynamic", "lib", "BrainFlowBluetooth.dll"));
        }
    }
}
