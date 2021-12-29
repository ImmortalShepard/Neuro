#pragma once

#include "CoreMinimal.h"

#include "SpeciesSettings.h"
#include "Engine/DataAsset.h"
#include "SlimeSettings.generated.h"

UENUM(BlueprintType)
enum class ESpawnMode : uint8
{
	Random,
	Point,
	InwardCircle,
	RandomCircle,
	CustomSpawn
};

UCLASS(BlueprintType)
class SLIMECOMPUTESHADERDECLARATION_API USlimeSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(ClampMin=0, UIMin=0))
	int32 NumAgents;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation)
	ESpawnMode SpawnMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(EditCondition="SpawnMode==ESpawnMode::Point||SpawnMode==ESpawnMode::InwardCircle||SpawnMode==ESpawnMode::RandomCircle", EditConditionHides, ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float SpawnPointX = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(EditCondition="SpawnMode==ESpawnMode::Point||SpawnMode==ESpawnMode::InwardCircle||SpawnMode==ESpawnMode::RandomCircle", EditConditionHides, ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float SpawnPointY = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(EditCondition="SpawnMode==ESpawnMode::InwardCircle||SpawnMode==ESpawnMode::RandomCircle", EditConditionHides, ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float SpawnCircleSize = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Trail)
	float TrailWeight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Trail)
	float DecayRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Trail)
	float DiffuseRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Species)
	TArray<FSpeciesSettings> SpeciesSettings;
};
