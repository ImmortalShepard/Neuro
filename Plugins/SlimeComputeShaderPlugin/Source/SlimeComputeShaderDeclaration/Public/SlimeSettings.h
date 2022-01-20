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
	OutwardCircle,
	RandomCircle,
	CustomSpawn
};

UENUM(BlueprintType)
enum class EMapBounds : uint8
{
	Texture,
	Circle
};

USTRUCT(BlueprintType)
struct SLIMECOMPUTESHADERDECLARATION_API FBoundsSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapBounds MapBounds = EMapBounds::Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="MapBounds==EMapBounds::Circle", EditConditionHides, ClampMin=0, UIMin=0))
	float CircleRadius = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="MapBounds==EMapBounds::Circle", EditConditionHides))
	FVector2D CircleCenter = FVector2D(0.5f, 0.5f);
};

UCLASS(BlueprintType)
class SLIMECOMPUTESHADERDECLARATION_API USlimeSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(ClampMin=0, UIMin=0))
	int32 NumAgents = 100000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation)
	ESpawnMode SpawnMode = ESpawnMode::Random;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(EditCondition="SpawnMode==ESpawnMode::Point||SpawnMode==ESpawnMode::InwardCircle||SpawnMode==ESpawnMode::OutwardCircle||SpawnMode==ESpawnMode::RandomCircle", EditConditionHides, ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float SpawnPointX = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(EditCondition="SpawnMode==ESpawnMode::Point||SpawnMode==ESpawnMode::InwardCircle||SpawnMode==ESpawnMode::OutwardCircle||SpawnMode==ESpawnMode::RandomCircle", EditConditionHides, ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float SpawnPointY = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation, meta=(EditCondition="SpawnMode==ESpawnMode::InwardCircle||SpawnMode==ESpawnMode::OutwardCircle||SpawnMode==ESpawnMode::RandomCircle", EditConditionHides, ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float SpawnCircleSize = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Simulation)
	FBoundsSettings BoundsSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Species)
	TArray<FSlimeSpeciesSettings> SpeciesSettings;
};
