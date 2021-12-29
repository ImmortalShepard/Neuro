#include "SlimeSimActor.h"
#include "SlimeComputeShaderDeclarationModule.h"

#include "GameFramework/GameUserSettings.h"

// Sets default values
ASlimeSimActor::ASlimeSimActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASlimeSimActor::BeginPlay()
{
	Super::BeginPlay();

	if (SlimeSettings == nullptr || SlimeSettings->SpeciesSettings.Num() == 0)
	{
		return;
	}

	Resolution = GEngine->GetGameUserSettings()->GetScreenResolution();
    if (RenderTarget)
    {
	    RenderTarget->ClearColor = FLinearColor::Black;
    	RenderTarget->TargetGamma = 0;
    	RenderTarget->InitCustomFormat(Resolution.X, Resolution.Y, PF_FloatRGBA, true);
    }

	TResourceArray<FSpeciesSettings> speciesSettings;
	speciesSettings.Append(SlimeSettings->SpeciesSettings);
	
	TResourceArray<FSlimeAgent> agents;
	agents.Reserve(SlimeSettings->NumAgents);
	switch (SlimeSettings->SpawnMode)
	{
	case ESpawnMode::Random:
		SpawnRandom(agents);
		break;
	case ESpawnMode::Point:
		SpawnPoint(agents);
		break;
	case ESpawnMode::InwardCircle:
		SpawnInwardCircle(agents);
		break;
	case ESpawnMode::RandomCircle:
		SpawnRandomCircle(agents);
		break;
	case ESpawnMode::CustomSpawn:
		SpawnCustom(agents);
		break;
	default:
		SpawnRandom(agents);
	}

	FSlimeComputeShaderDeclarationModule::Get().BeginRendering(speciesSettings, agents);

	FSlimeComputeShaderParameters slimeParameters(RenderTarget);
	slimeParameters.NumSpecies = SlimeSettings->SpeciesSettings.Num();
	slimeParameters.NumAgents = SlimeSettings->NumAgents;
	slimeParameters.DecayRate = SlimeSettings->DecayRate;
	slimeParameters.DiffuseRate = SlimeSettings->DiffuseRate;
	slimeParameters.TrailWeight = SlimeSettings->TrailWeight;
	
	FSlimeComputeShaderDeclarationModule::Get().UpdateParameters(slimeParameters);
}

void ASlimeSimActor::SpawnRandom(TResourceArray<FSlimeAgent>& SlimeAgents) const
{
	for (int i = 0; i < SlimeSettings->NumAgents; ++i)
	{
		FSlimeAgent newAgent;
		newAgent.Position = FVector2D(FMath::FRandRange(0, Resolution.X), FMath::FRandRange(0, Resolution.Y));
		newAgent.Angle = FMath::FRand() * PI * 2;
		AssignSpecies(newAgent, i);

		SlimeAgents.Add(newAgent);
	}
}

void ASlimeSimActor::SpawnPoint(TResourceArray<FSlimeAgent>& SlimeAgents) const
{
	const FVector2D spawnPoint = FVector2D(SlimeSettings->SpawnPointX * Resolution.X, SlimeSettings->SpawnPointY * Resolution.Y);
	for (int i = 0; i < SlimeSettings->NumAgents; ++i)
	{
		FSlimeAgent newAgent;
		newAgent.Position = spawnPoint;
		newAgent.Angle = FMath::FRand() * PI * 2;
		AssignSpecies(newAgent, i);

		SlimeAgents.Add(newAgent);
	}
}

void ASlimeSimActor::SpawnInwardCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const
{
	FVector2D center = FVector2D(SlimeSettings->SpawnPointX * Resolution.X, SlimeSettings->SpawnPointY * Resolution.Y);
	float circleRadius = SlimeSettings->SpawnCircleSize * Resolution.GetMin() * 0.5f;
	for (int i = 0; i < SlimeSettings->NumAgents; ++i)
	{
		FSlimeAgent newAgent;
		newAgent.Position = center + FMath::RandPointInCircle(circleRadius);
		const FVector2D angleVector = (center - newAgent.Position).GetSafeNormal();
		newAgent.Angle = angleVector.IsZero() ? FMath::FRand() * PI * 2 : FMath::Atan2(angleVector.Y, angleVector.X);
		AssignSpecies(newAgent, i);

		SlimeAgents.Add(newAgent);
	}
}

void ASlimeSimActor::SpawnRandomCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const
{
	FVector2D center = FVector2D(SlimeSettings->SpawnPointX * Resolution.X, SlimeSettings->SpawnPointY * Resolution.Y);
	float circleRadius = SlimeSettings->SpawnCircleSize * Resolution.GetMin() * 0.5f;
	for (int i = 0; i < SlimeSettings->NumAgents; ++i)
	{
		FSlimeAgent newAgent;
		newAgent.Position = center + FMath::RandPointInCircle(circleRadius);
		newAgent.Angle = FMath::FRand() * PI * 2;
		AssignSpecies(newAgent, i);

		SlimeAgents.Add(newAgent);
	}
}

void ASlimeSimActor::SpawnCustom(TResourceArray<FSlimeAgent>& SlimeAgents)
{
	for (int i = 0; i < SlimeSettings->NumAgents; ++i)
	{
		SlimeAgents.Add(CustomSpawn(i));
	}
}

void ASlimeSimActor::AssignSpecies(FSlimeAgent& SlimeAgent, int Index) const
{
	const int numSpecies = SlimeSettings->SpeciesSettings.Num();
	const int speciesInterval = SlimeSettings->NumAgents / numSpecies;
	int speciesIndex = Index / speciesInterval;
	speciesIndex = speciesIndex >= numSpecies ? numSpecies - 1 : speciesIndex;
	SlimeAgent.SpeciesIndex = speciesIndex;
	SlimeAgent.SpeciesMask = FIntVector4(speciesIndex == 0 ? 1 : 0, speciesIndex == 1 ? 1 : 0, speciesIndex == 2 ? 1 : 0, 0);
}

// Called every frame
void ASlimeSimActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FSlimeTimeParameters timeParameters;
	timeParameters.DeltaTime = DeltaTime;
	UWorld* world = GetWorld();
	if (world)
	{
		timeParameters.Time = world->GetTimeSeconds();
	}
	
	FSlimeComputeShaderDeclarationModule::Get().UpdateTimeParameters(timeParameters);
}

void ASlimeSimActor::BeginDestroy()
{
	FSlimeComputeShaderDeclarationModule::Get().EndRendering();
	Super::BeginDestroy();
}

