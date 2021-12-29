#pragma once

#include "CoreMinimal.h"
#include "SpeciesSettings.generated.h"

USTRUCT(BlueprintType)
struct SLIMECOMPUTESHADERDECLARATION_API FSpeciesSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
    float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
    float TurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Sensor)
    float SensorAngleDegrees;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Sensor)
    float SensorOffsetDst;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Sensor)
    int SensorSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
	FLinearColor Color = FLinearColor::White;
};