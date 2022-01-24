// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "RenderGraphResources.h"
#include "SlimeAgent.h"
#include "SlimeSettings.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"

struct SLIMECOMPUTESHADERDECLARATION_API FSlimeComputeShaderParameters
{
	UTextureRenderTarget2D* RenderTarget;
	uint32 NumSpecies;
	FTrailSettings TrailSettings;
	uint32 NumAgents;
	EMapBounds MapBounds;
	float CircleRadius;
	FVector2D CircleCenter;

	FIntPoint GetRenderTargetSize() const
	{
		return CachedRenderTargetSize;
	}

	FSlimeComputeShaderParameters() {}
	FSlimeComputeShaderParameters(UTextureRenderTarget2D* InRenderTarget)
		: RenderTarget(InRenderTarget)
	{
		CachedRenderTargetSize = RenderTarget ? FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY) : FIntPoint::ZeroValue;
	}

private:
	FIntPoint CachedRenderTargetSize;
};

struct SLIMECOMPUTESHADERDECLARATION_API FSlimeTimeParameters
{
	float DeltaTime;
	float Time;

	FSlimeTimeParameters() {}
};

class SLIMECOMPUTESHADERDECLARATION_API FSlimeComputeShaderDeclarationModule : public IModuleInterface
{
public:
	static inline FSlimeComputeShaderDeclarationModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FSlimeComputeShaderDeclarationModule>("SlimeComputeShaderDeclaration");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("SlimeComputeShaderDeclaration");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void BeginRendering(TResourceArray<FSpeciesSettings>& SpeciesSettings, TResourceArray<FSlimeAgent>& Agents);

	void EndRendering();

	void UpdateParameters(const FSlimeComputeShaderParameters& DrawParameters);

	void UpdateSpeciesSettings(TResourceArray<FSpeciesSettings>& SpeciesSettings);

	void UpdateTimeParameters(const FSlimeTimeParameters& TimeParameters);

private:
	TResourceArray<FSpeciesSettings> SpeciesSettingsArray;
	TResourceArray<FSlimeAgent> AgentArray;

	FStructuredBufferRHIRef SpeciesSettingsBuffer;
	FShaderResourceViewRHIRef SpeciesSettingsBufferSRV;
	FStructuredBufferRHIRef AgentsBuffer;
	FUnorderedAccessViewRHIRef AgentsBufferUAV;

	FTexture2DRHIRef TrailMap;
	FUnorderedAccessViewRHIRef TrailMapUAV;
	FTexture2DRHIRef DiffuseTrailMap;
	FUnorderedAccessViewRHIRef DiffuseTrailMapUAV;
	
	FSlimeComputeShaderParameters CachedSlimeComputeShaderParameters;
	FSlimeTimeParameters SlimeTimeParameters;
	FDelegateHandle OnPostResolvedSceneColorHandle;
	FCriticalSection RenderEveryFrameLock;
	volatile bool CachedParametersValid;
	volatile bool SpeciesValid;
	
	void PostResolveSceneColor_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneRenderTargets& SceneContext);
	void Draw_RenderThread();
};
