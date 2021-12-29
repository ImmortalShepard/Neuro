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
	bCachedParametersValid = false;

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
	AgentArray = MoveTemp(Agents);

	bCachedParametersValid = false;

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
	RenderEveryFrameLock.Lock();
	CachedSlimeComputeShaderParameters = DrawParameters;
	bCachedParametersValid = true;
	RenderEveryFrameLock.Unlock();
}

void FSlimeComputeShaderDeclarationModule::UpdateTimeParameters(const FSlimeTimeParameters& TimeParameters)
{
	SlimeTimeParameters = TimeParameters;
}


void FSlimeComputeShaderDeclarationModule::PostResolveSceneColor_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneRenderTargets& SceneContext)
{
	if (!bCachedParametersValid)
	{
		return;
	}

	RenderEveryFrameLock.Lock();
	FSlimeComputeShaderParameters Copy = CachedSlimeComputeShaderParameters;
	RenderEveryFrameLock.Unlock();

	Draw_RenderThread(Copy);
}

void FSlimeComputeShaderDeclarationModule::Draw_RenderThread(const FSlimeComputeShaderParameters& DrawParameters)
{
	check(IsInRenderingThread());

	if (!DrawParameters.RenderTarget)
	{
		return;
	}

	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_Render);
	SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_Render);

	/*if (!TrailMap.IsValid())
	{
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(DrawParameters.GetRenderTargetSize(), PF_FloatRGBA, FClearValueBinding::Black, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		ComputeShaderOutputDesc.DebugName = TEXT("SlimePlugin_TrailMap");
		GRenderTargetPool.FindFreeElement(RHICmdList, ComputeShaderOutputDesc, TrailMap, TEXT("SlimePlugin_TrailMap"));
	}*/

	if (!TrailMap.IsValid())
	{
		FRHIResourceCreateInfo CreateInfo(TEXT("SlimePlugin_SpeciesSettingsBuffer"));
		CreateInfo.ClearValueBinding = FClearValueBinding::Black;
		TrailMap = RHICreateTexture2D(DrawParameters.GetRenderTargetSize().X, DrawParameters.GetRenderTargetSize().Y, PF_FloatRGBA, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		TrailMapUAV = RHICreateUnorderedAccessView(TrailMap);
	}

	/*if (!DiffuseTrailMap.IsValid())
	{
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(DrawParameters.GetRenderTargetSize(), PF_FloatRGBA, FClearValueBinding::Black, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		ComputeShaderOutputDesc.DebugName = TEXT("SlimePlugin_DiffuseTrailMap");
		GRenderTargetPool.FindFreeElement(RHICmdList, ComputeShaderOutputDesc, DiffuseTrailMap, TEXT("SlimePlugin_DiffuseTrailMap"));
	}*/

	if (!DiffuseTrailMap.IsValid())
	{
		FRHIResourceCreateInfo CreateInfo(TEXT("SlimePlugin_SpeciesSettingsBuffer"));
		CreateInfo.ClearValueBinding = FClearValueBinding::Black;
		DiffuseTrailMap = RHICreateTexture2D(DrawParameters.GetRenderTargetSize().X, DrawParameters.GetRenderTargetSize().Y, PF_FloatRGBA, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		DiffuseTrailMapUAV = RHICreateUnorderedAccessView(DiffuseTrailMap);
	}
	
	if (!SpeciesSettingsBuffer.IsValid())
	{
		FRHIResourceCreateInfo CreateInfo(TEXT("SlimePlugin_SpeciesSettingsBuffer"));
		CreateInfo.ResourceArray = &SpeciesSettingsArray;
		const uint32 SpeciesSettingsSize = sizeof(FSpeciesSettings);
		const uint32 SpeciesSettingsBufferSize = SpeciesSettingsArray.Num() * SpeciesSettingsSize;
		SpeciesSettingsBuffer = RHICreateStructuredBuffer(SpeciesSettingsSize, SpeciesSettingsBufferSize, BUF_StructuredBuffer | BUF_ShaderResource, ERHIAccess::SRVMask, CreateInfo);
		SpeciesSettingsBufferSRV = RHICreateShaderResourceView(SpeciesSettingsBuffer);
	}

	if (!AgentsBuffer.IsValid())
	{
		FRHIResourceCreateInfo CreateInfo(TEXT("SlimePlugin_AgentBuffer"));
		CreateInfo.ResourceArray = &AgentArray;
		const uint32 AgentSize = sizeof(FSlimeAgent);
		const uint32 AgentBufferSize = AgentArray.Num() * AgentSize;
		AgentsBuffer = RHICreateStructuredBuffer(AgentSize, AgentBufferSize, BUF_StructuredBuffer | BUF_UnorderedAccess, ERHIAccess::SRVMask, CreateInfo);
		AgentsBufferUAV = RHICreateUnorderedAccessView(AgentsBuffer, false, false);
	}

	FSlimeSim::RunComputeShader_RenderThread(RHICmdList, DrawParameters, SlimeTimeParameters, TrailMapUAV, DiffuseTrailMapUAV, SpeciesSettingsBufferSRV, AgentsBufferUAV);
	TrailMap.Swap(DiffuseTrailMap);
	TrailMapUAV.Swap(DiffuseTrailMapUAV);
	FSlimeSim::DrawToRenderTarget_RenderThread(RHICmdList, DrawParameters, TrailMap, SpeciesSettingsBufferSRV);
}