// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SlimeComputeShaderDeclaration : ModuleRules
{
	public SlimeComputeShaderDeclaration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[]
		{
			"SlimeComputeShaderDeclaration/Public"
		});
		
		PrivateIncludePaths.AddRange(new string[]
		{
			"SlimeComputeShaderDeclaration/Private"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Renderer",
			"RenderCore",
			"RHI",
			"Projects"
		});
	}
}
