#pragma once

#include "CoreMinimal.h"
#include "SlimeAgent.generated.h"

USTRUCT(BlueprintType)
struct SLIMECOMPUTESHADERDECLARATION_API FSlimeAgent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector4 SpeciesMask;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpeciesIndex;
};