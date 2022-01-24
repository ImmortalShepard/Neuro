// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeComputeShaderDeclarationModule.h"

#include "SlimeSim.h"

#include "RenderTargetPool.h"
#include "Interfaces/IPluginManager.h"

IMPLEMENT_MODULE(FSlimeComputeShaderDeclarationModule, SlimeComputeShaderDeclaration)

// Declare some GPU stats so we can track them later
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Render, TEXT("ShaderPlugin: Root Render"));
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Compute, TEXT("ShaderPlugin: Render Compute Shader"));
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Pixel, TEXT("ShaderPlugin: Render Pixel Shader"));

void FSlimeComputeShaderDeclarationModule::StartupModule()
{
	OnPostResolvedSceneColorHandle.Reset();
	CachedParametersValid = false;
	SpeciesValid = false;

	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SlimeComputeShaderPlugin"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/SlimeShaders"), PluginShaderDir);
}

void FSlimeComputeShaderDeclarationModule::ShutdownModule()
{
	EndRendering();
}

void FSlimeComputeShaderDeclarationModule::BeginRendering(TResourceArray<FSpeciesSettings>& SpeciesSettings, TResourceArray<FSlimeAgent>& Agents)
{
	if (OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}


	SpeciesSettingsArray = MoveTemp(SpeciesSettings);
	SpeciesValid = true;
	AgentArray = MoveTemp(Agents);

	CachedParametersValid = false;

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &FSlimeComputeShaderDeclarationModule::PostResolveSceneColor_RenderThread);
	}
}

void FSlimeComputeShaderDeclarationModule::EndRendering()
{
	if (!OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	SpeciesSettingsBuffer.SafeRelease();
	SpeciesSettingsBufferSRV.SafeRelease();
	AgentsBuffer.SafeRelease();
	AgentsBufferUAV.SafeRelease();
	TrailMap.SafeRelease();
	TrailMapUAV.SafeRelease();

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
	}

	OnPostResolvedSceneColorHandle.Reset();
}

void FSlimeComputeShaderDeclarationModule::UpdateParameters(const FSlimeComputeShaderParameters& DrawParameters)
{
	//RenderEveryFrameLock.Lock();
	CachedSlimeComputeShaderParameters = DrawParameters;
	CachedParametersValid = true;
	//RenderEveryFrameLock.Unlock();
}

void FSlimeComputeShaderDeclarationModule::UpdateSpeciesSettings(TResourceArray<FSpeciesSettings>& SpeciesSettings)
{
	//RenderEveryFrameLock.Lock();
	SpeciesSettingsArray = MoveTemp(SpeciesSettings);
	SpeciesValid = false;
	//RenderEveryFrameLock.Unlock();
}

void FSlimeComputeShaderDeclarationModule::UpdateTimeParameters(const FSlimeTimeParameters& TimeParameters)
{
	SlimeTimeParameters = TimeParameters;
}


void FSlimeComputeShaderDeclarationModule::PostResolveSceneColor_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneRenderTargets& SceneContext)
{
	if (!CachedParametersValid)
	{
		return;
	}

	//FSlimeComputeShaderParameters copy = CachedSlimeComputeShaderParameters;

	Draw_RenderThread();
}

void FSlimeComputeShaderDeclarationModule::Draw_RenderThread()
{
	check(IsInRenderingThread());

	if (!CachedSlimeComputeShaderParameters.RenderTarget)
	{
		return;
	}

	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_Render);
	SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_Render);

	if (!TrailMap.IsValid())
	{
		FRHIResourceCreateInfo createInfo(TEXT("SlimePlugin_TrailMap"));
		createInfo.ClearValueBinding = FClearValueBinding::Black;
		TrailMap = RHICreateTexture2D(CachedSlimeComputeShaderParameters.GetRenderTargetSize().X, CachedSlimeComputeShaderParameters.GetRenderTargetSize().Y, PF_FloatRGBA, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, createInfo);
		TrailMapUAV = RHICreateUnorderedAccessView(TrailMap);
	}

	if (!DiffuseTrailMap.IsValid())
	{
		FRHIResourceCreateInfo createInfo(TEXT("SlimePlugin_DiffuseTrailMap"));
		createInfo.ClearValueBinding = FClearValueBinding::Black;
		DiffuseTrailMap = RHICreateTexture2D(CachedSlimeComputeShaderParameters.GetRenderTargetSize().X, CachedSlimeComputeShaderParameters.GetRenderTargetSize().Y, PF_FloatRGBA, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, createInfo);
		DiffuseTrailMapUAV = RHICreateUnorderedAccessView(DiffuseTrailMap);
	}

	if (!SpeciesSettingsBuffer.IsValid())
	{
		FRHIResourceCreateInfo createInfo(TEXT("SlimePlugin_SpeciesSettingsBuffer"));
		createInfo.ResourceArray = &SpeciesSettingsArray;
		const uint32 speciesSettingsSize = sizeof(FSpeciesSettings);
		const uint32 speciesSettingsBufferSize = SpeciesSettingsArray.Num() * speciesSettingsSize;
		SpeciesSettingsBuffer = RHICreateStructuredBuffer(speciesSettingsSize, speciesSettingsBufferSize, BUF_StructuredBuffer | BUF_ShaderResource | BUF_Dynamic, ERHIAccess::SRVMask, createInfo);
		SpeciesSettingsBufferSRV = RHICreateShaderResourceView(SpeciesSettingsBuffer);
		SpeciesValid = true;
	}
	else if (!SpeciesValid)
	{
		const uint32 speciesSettingsSize = sizeof(FSpeciesSettings);
		const uint32 speciesSettingsBufferSize = SpeciesSettingsArray.Num() * speciesSettingsSize;
		void* lockedData = RHILockStructuredBuffer(SpeciesSettingsBuffer, 0, speciesSettingsBufferSize, RLM_WriteOnly);
		FPlatformMemory::Memcpy(lockedData, SpeciesSettingsArray.GetResourceData(), speciesSettingsBufferSize);
		RHIUnlockStructuredBuffer(SpeciesSettingsBuffer);
		SpeciesValid = true;
	}

	if (!AgentsBuffer.IsValid())
	{
		FRHIResourceCreateInfo createInfo(TEXT("SlimePlugin_AgentsBuffer"));
		createInfo.ResourceArray = &AgentArray;
		const uint32 agentSize = sizeof(FSlimeAgent);
		const uint32 agentBufferSize = AgentArray.Num() * agentSize;
		AgentsBuffer = RHICreateStructuredBuffer(agentSize, agentBufferSize, BUF_StructuredBuffer | BUF_UnorderedAccess, ERHIAccess::UAVMask, createInfo);
		AgentsBufferUAV = RHICreateUnorderedAccessView(AgentsBuffer, false, false);
	}

	FSlimeSim::RunComputeShader_RenderThread(RHICmdList, CachedSlimeComputeShaderParameters, SlimeTimeParameters, TrailMapUAV, DiffuseTrailMapUAV, SpeciesSettingsBufferSRV, AgentsBufferUAV);
	TrailMap.Swap(DiffuseTrailMap);
	TrailMapUAV.Swap(DiffuseTrailMapUAV);
	FSlimeSim::DrawToRenderTarget_RenderThread(RHICmdList, CachedSlimeComputeShaderParameters, TrailMap, SpeciesSettingsBufferSRV);
}
