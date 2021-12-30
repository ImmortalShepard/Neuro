#include "SlimeSim.h"

#include "CommonRenderResources.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ShaderParameterStruct.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ShaderCompilerCore.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 32

class FSlimeSimCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSlimeSimCS);
	SHADER_USE_PARAMETER_STRUCT(FSlimeSimCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_SRV(StructuredBuffer<SpeciesSettings>, SpeciesSettingsBuffer)
		SHADER_PARAMETER(int, NumSpecies)
		SHADER_PARAMETER(FVector4, TrailWeight)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<SlimeAgent>, Agents)
		SHADER_PARAMETER(uint32, NumAgents)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER(FVector2D, TextureSize)
		SHADER_PARAMETER(float, DeltaTime)
		SHADER_PARAMETER(float, Time)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		/*OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);*/

		OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
	}
};

class FDiffuseCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDiffuseCS);
	SHADER_USE_PARAMETER_STRUCT(FDiffuseCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FVector4, DecayRate)
		SHADER_PARAMETER(FVector4, DiffuseRate)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER(FVector2D, TextureSize)
		SHADER_PARAMETER(float, DeltaTime)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, DiffuseTrailMap)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
	}
};

class FSimpleScreenVertexBuffer : public FVertexBuffer
{
public:
	/** Initialize the RHI for this rendering resource */
	virtual void InitRHI() override
	{
		TResourceArray<FFilterVertex, VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(6);

		Vertices[0].Position = FVector4(-1, 1, 0, 1);
		Vertices[0].UV = FVector2D(0, 0);

		Vertices[1].Position = FVector4(1, 1, 0, 1);
		Vertices[1].UV = FVector2D(1, 0);

		Vertices[2].Position = FVector4(-1, -1, 0, 1);
		Vertices[2].UV = FVector2D(0, 1);

		Vertices[3].Position = FVector4(1, -1, 0, 1);
		Vertices[3].UV = FVector2D(1, 1);

		// Create vertex buffer. Fill buffer with initial data upon creation
		FRHIResourceCreateInfo CreateInfo(&Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};
TGlobalResource<FSimpleScreenVertexBuffer> GSimpleScreenVertexBuffer;

class FSlimeVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSlimeVS);

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	FSlimeVS() { }
	FSlimeVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer) { }
};

class FSlimePS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSlimePS);
	SHADER_USE_PARAMETER_STRUCT(FSlimePS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_SRV(StructuredBuffer<SpeciesSettings>, SpeciesSettingsBuffer)
		SHADER_PARAMETER(int, NumSpecies)
		SHADER_PARAMETER(FVector2D, TextureSize)
		SHADER_PARAMETER_TEXTURE(Texture2D<FVector4>, TrailMap)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
	}
};

IMPLEMENT_GLOBAL_SHADER(FSlimeSimCS, "/SlimeShaders/Private/SlimeSim.usf", "Update", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FDiffuseCS, "/SlimeShaders/Private/SlimeDiffuse.usf", "Diffuse", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FSlimeVS, "/SlimeShaders/Private/SlimePixelShader.usf", "SlimeVertexShader", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FSlimePS, "/SlimeShaders/Private/SlimePixelShader.usf", "SlimePixelShader", SF_Pixel);

void FSlimeSim::RunComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList,
	const FSlimeComputeShaderParameters& DrawParameters, const FSlimeTimeParameters& TimeParameters,
	FUnorderedAccessViewRHIRef TrailMap, FUnorderedAccessViewRHIRef DiffuseTrailMap,
	FShaderResourceViewRHIRef SpeciesSettingsBufferSRV, FUnorderedAccessViewRHIRef AgentsBufferUAV)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_ComputeShader);
	SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_Compute);

	RHICmdList.TransitionResource(ERHIAccess::ERWBarrier, EResourceTransitionPipeline::EGfxToCompute, TrailMap);
	RHICmdList.TransitionResource(ERHIAccess::ERWBarrier, EResourceTransitionPipeline::EGfxToCompute, AgentsBufferUAV);
	RHICmdList.TransitionResource(ERHIAccess::ERWBarrier, EResourceTransitionPipeline::EGfxToCompute, DiffuseTrailMap);

	FSlimeSimCS::FParameters SlimePassParameters;
	SlimePassParameters.SpeciesSettingsBuffer = SpeciesSettingsBufferSRV;
	SlimePassParameters.NumSpecies = DrawParameters.NumSpecies;
	SlimePassParameters.TrailWeight = DrawParameters.TrailSettings.TrailWeight;
	SlimePassParameters.Agents = AgentsBufferUAV;
	SlimePassParameters.NumAgents = DrawParameters.NumAgents;
	SlimePassParameters.TrailMap = TrailMap;
	SlimePassParameters.TextureSize = FVector2D(DrawParameters.GetRenderTargetSize().X, DrawParameters.GetRenderTargetSize().Y);
	SlimePassParameters.DeltaTime = TimeParameters.DeltaTime;
	SlimePassParameters.Time = TimeParameters.Time;

	TShaderMapRef<FSlimeSimCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(static_cast<int>(DrawParameters.NumAgents), 64), 1, 1);

	FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, SlimePassParameters, GroupCounts);
	
	FDiffuseCS::FParameters DiffusePassParameters;
	DiffusePassParameters.DecayRate = DrawParameters.TrailSettings.DecayRate;
	DiffusePassParameters.DiffuseRate = DrawParameters.TrailSettings.DiffuseRate;
	DiffusePassParameters.TrailMap = TrailMap;
	DiffusePassParameters.TextureSize = FVector2D(DrawParameters.GetRenderTargetSize().X, DrawParameters.GetRenderTargetSize().Y);
	DiffusePassParameters.DeltaTime = TimeParameters.DeltaTime;
	DiffusePassParameters.DiffuseTrailMap = DiffuseTrailMap;

	TShaderMapRef<FDiffuseCS> DiffuseShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	GroupCounts = FIntVector(FMath::DivideAndRoundUp(DrawParameters.GetRenderTargetSize().X, 8), FMath::DivideAndRoundUp(DrawParameters.GetRenderTargetSize().Y, 8), 1);
	
	FComputeShaderUtils::Dispatch(RHICmdList, DiffuseShader, DiffusePassParameters, GroupCounts);

	//TrailMap.Swap(DiffuseTrailMap);
}

void FSlimeSim::DrawToRenderTarget_RenderThread(FRHICommandListImmediate& RHICmdList, const FSlimeComputeShaderParameters DrawParameters, FTexture2DRHIRef TrailMap, FShaderResourceViewRHIRef SpeciesSettingsBufferSRV)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_PixelShader); // Used to gather CPU profiling data for the UE4 session frontend
	SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_Pixel); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	FRHIRenderPassInfo RenderPassInfo(DrawParameters.RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), ERenderTargetActions::Clear_Store);
	RHICmdList.BeginRenderPass(RenderPassInfo, TEXT("SlimePlugin_OutputToRenderTarget"));

	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef<FSlimeVS> VertexShader(ShaderMap);
	TShaderMapRef<FSlimePS> PixelShader(ShaderMap);

	// Set the graphic pipeline state.
	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	// Setup the pixel shader
	FSlimePS::FParameters PassParameters;
	PassParameters.SpeciesSettingsBuffer = SpeciesSettingsBufferSRV;
	PassParameters.NumSpecies = DrawParameters.NumSpecies;
	PassParameters.TextureSize = FVector2D(DrawParameters.GetRenderTargetSize().X, DrawParameters.GetRenderTargetSize().Y);
	PassParameters.TrailMap = TrailMap;
	SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PassParameters);

	// Draw
	RHICmdList.SetStreamSource(0, GSimpleScreenVertexBuffer.VertexBufferRHI, 0);
	RHICmdList.DrawPrimitive(0, 2, 1);

	// Resolve render target
	RHICmdList.CopyToResolveTarget(DrawParameters.RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), DrawParameters.RenderTarget->GetRenderTargetResource()->TextureRHI, FResolveParams());

	RHICmdList.EndRenderPass();
}
