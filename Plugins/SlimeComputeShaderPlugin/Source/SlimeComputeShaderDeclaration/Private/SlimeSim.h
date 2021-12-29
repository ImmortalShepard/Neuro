#pragma once

#include "CoreMinimal.h"
#include "SlimeComputeShaderDeclarationModule.h"

class FSlimeSim
{
public:
	static void RunComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList, const FSlimeComputeShaderParameters& DrawParameters, const FSlimeTimeParameters& TimeParameters, FUnorderedAccessViewRHIRef TrailMap, FUnorderedAccessViewRHIRef DiffuseTrailMap, FShaderResourceViewRHIRef SpeciesSettingsBufferSRV, FUnorderedAccessViewRHIRef AgentsBufferUAV);
	static void DrawToRenderTarget_RenderThread(FRHICommandListImmediate& RHICmdList, const FSlimeComputeShaderParameters DrawParameters, FTexture2DRHIRef TrailMap, FShaderResourceViewRHIRef SpeciesSettingsBufferSRV);
};
