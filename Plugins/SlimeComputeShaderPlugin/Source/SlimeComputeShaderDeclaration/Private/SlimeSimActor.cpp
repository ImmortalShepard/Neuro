#include "SlimeSimActor.h"

#include "GameFramework/GameUserSettings.h"

#define MAX_NUMBER_OF_SPECIES 4

// Sets default values
ASlimeSimActor::ASlimeSimActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

FSlimeAgent& ASlimeSimActor::AssignSpeciesByListIndex(FSlimeAgent& SlimeAgent, int ListIndex) const
{
	AssignSpecies(SlimeAgent, ListIndex);
	return SlimeAgent;
}

void ASlimeSimActor::UpdateSlimeSim(const TArray<FSlimeSpeciesSettings>& SpeciesSettings, const FBoundsSettings& BoundsSettings)
{
	FTrailSettings trailSettings;
	if (SlimeSettings->SpeciesSettings.Num() == SpeciesSettings.Num())
	{
		TResourceArray<FSpeciesSettings> speciesSettings;
		speciesSettings.Reserve(SpeciesSettings.Num());
		for (FSlimeSpeciesSettings SpeciesSetting : SpeciesSettings)
		{
			speciesSettings.Add(SpeciesSetting.SpeciesSettings);
		}
		FSlimeComputeShaderDeclarationModule::Get().UpdateSpeciesSettings(speciesSettings);
		
		for (int i = 0; i < SpeciesSettings.Num(); ++i)
		{
			trailSettings.TrailWeight[i] = SpeciesSettings[i].TrailWeight;
			trailSettings.DecayRate[i] = SpeciesSettings[i].DecayRate;
			trailSettings.DiffuseRate[i] = SpeciesSettings[i].DiffuseRate;
		}
	}

	DrawParameters.TrailSettings = trailSettings;
	
	DrawParameters.MapBounds = BoundsSettings.MapBounds;
	DrawParameters.CircleRadius = BoundsSettings.CircleRadius * Resolution.GetMin();
	DrawParameters.CircleCenter = BoundsSettings.CircleCenter * Resolution;

	FSlimeComputeShaderDeclarationModule::Get().UpdateParameters(DrawParameters);
}

void ASlimeSimActor::UpdateSlimeSimSpecies(const TArray<FSlimeSpeciesSettings>& SpeciesSettings)
{
	FTrailSettings trailSettings;
	if (SlimeSettings->SpeciesSettings.Num() == SpeciesSettings.Num())
	{
		TResourceArray<FSpeciesSettings> speciesSettings;
		speciesSettings.Reserve(SpeciesSettings.Num());
		for (FSlimeSpeciesSettings SpeciesSetting : SpeciesSettings)
		{
			speciesSettings.Add(SpeciesSetting.SpeciesSettings);
		}
		FSlimeComputeShaderDeclarationModule::Get().UpdateSpeciesSettings(speciesSettings);
		
		for (int i = 0; i < SpeciesSettings.Num(); ++i)
		{
			trailSettings.TrailWeight[i] = SpeciesSettings[i].TrailWeight;
			trailSettings.DecayRate[i] = SpeciesSettings[i].DecayRate;
			trailSettings.DiffuseRate[i] = SpeciesSettings[i].DiffuseRate;
		}
	}

	DrawParameters.TrailSettings = trailSettings;

	FSlimeComputeShaderDeclarationModule::Get().UpdateParameters(DrawParameters);
}

void ASlimeSimActor::UpdateSlimeSimBounds(const FBoundsSettings& BoundsSettings)
{
	DrawParameters.MapBounds = BoundsSettings.MapBounds;
	DrawParameters.CircleRadius = BoundsSettings.CircleRadius * Resolution.GetMin();
	DrawParameters.CircleCenter = BoundsSettings.CircleCenter * Resolution;

	FSlimeComputeShaderDeclarationModule::Get().UpdateParameters(DrawParameters);
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

	if (SlimeSettings->SpeciesSettings.Num() > MAX_NUMBER_OF_SPECIES)
	{
		SlimeSettings->SpeciesSettings.SetNum(MAX_NUMBER_OF_SPECIES);
	}
	TResourceArray<FSpeciesSettings> speciesSettings;
	speciesSettings.Reserve(SlimeSettings->SpeciesSettings.Num());
	for (FSlimeSpeciesSettings SpeciesSetting : SlimeSettings->SpeciesSettings)
	{
		speciesSettings.Add(SpeciesSetting.SpeciesSettings);
	}

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
	case ESpawnMode::OutwardCircle:
		SpawnOutwardCircle(agents);
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

	DrawParameters = FSlimeComputeShaderParameters(RenderTarget);
	DrawParameters.NumSpecies = SlimeSettings->SpeciesSettings.Num();
	FTrailSettings trailSettings;
	for (int i = 0; i < SlimeSettings->SpeciesSettings.Num(); ++i)
	{
		trailSettings.TrailWeight[i] = SlimeSettings->SpeciesSettings[i].TrailWeight;
		trailSettings.DecayRate[i] = SlimeSettings->SpeciesSettings[i].DecayRate;
		trailSettings.DiffuseRate[i] = SlimeSettings->SpeciesSettings[i].DiffuseRate;
	}
	DrawParameters.TrailSettings = trailSettings;
	DrawParameters.NumAgents = SlimeSettings->NumAgents;
	DrawParameters.MapBounds = SlimeSettings->BoundsSettings.MapBounds;
	DrawParameters.CircleRadius = SlimeSettings->BoundsSettings.CircleRadius * Resolution.GetMin();
	DrawParameters.CircleCenter = SlimeSettings->BoundsSettings.CircleCenter * Resolution;

	FSlimeComputeShaderDeclarationModule::Get().UpdateParameters(DrawParameters);
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

void ASlimeSimActor::SpawnOutwardCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const
{
	FVector2D center = FVector2D(SlimeSettings->SpawnPointX * Resolution.X, SlimeSettings->SpawnPointY * Resolution.Y);
	float circleRadius = SlimeSettings->SpawnCircleSize * Resolution.GetMin() * 0.5f;
	for (int i = 0; i < SlimeSettings->NumAgents; ++i)
	{
		FSlimeAgent newAgent;
		newAgent.Position = center + FMath::RandPointInCircle(circleRadius);
		const FVector2D angleVector = (newAgent.Position - center).GetSafeNormal();
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

void ASlimeSimActor::AssignSpecies(FSlimeAgent& SlimeAgent, int ListIndex) const
{
	const int numSpecies = SlimeSettings->SpeciesSettings.Num();
	const int speciesInterval = SlimeSettings->NumAgents / numSpecies;
	int speciesIndex = ListIndex / speciesInterval;
	speciesIndex = speciesIndex >= numSpecies ? numSpecies - 1 : speciesIndex;
	SlimeAgent.SetSpeciesIndex(speciesIndex);
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

