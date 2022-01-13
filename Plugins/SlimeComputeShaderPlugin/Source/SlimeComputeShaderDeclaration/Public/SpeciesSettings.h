#pragma once

#include "CoreMinimal.h"
#include "SpeciesSettings.generated.h"

USTRUCT(BlueprintType)
struct SLIMECOMPUTESHADERDECLARATION_API FSpeciesSettings
{
	GENERATED_BODY()
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    float MoveSpeed = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    float TurnSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sensor)
    float SensorAngleDegrees = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sensor)
    float SensorOffsetDst = 35;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sensor)
    int SensorSize = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Display)
	FLinearColor Color = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct SLIMECOMPUTESHADERDECLARATION_API FSlimeSpeciesSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Species)
	FSpeciesSettings SpeciesSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trail)
	float TrailWeight = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trail)
	float DecayRate = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trail)
	float DiffuseRate = 0.5f;
};

struct SLIMECOMPUTESHADERDECLARATION_API FTrailSettings
{
	FVector4 TrailWeight = FVector4(0,0,0,0);
	FVector4 DecayRate = FVector4(0,0,0,0);
	FVector4 DiffuseRate = FVector4(0,0,0,0);
};