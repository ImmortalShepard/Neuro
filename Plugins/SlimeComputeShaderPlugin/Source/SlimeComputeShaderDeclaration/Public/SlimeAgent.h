#pragma once

#include "CoreMinimal.h"
#include "SlimeAgent.generated.h"

USTRUCT(BlueprintType)
struct SLIMECOMPUTESHADERDECLARATION_API FSlimeAgent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Position = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle = 0;
	FIntVector4 SpeciesMask = FIntVector4(1,0,0,0);
	int SpeciesIndex = 0;

	void SetSpeciesIndex(int Index)
	{
		SpeciesIndex = Index;
		SpeciesMask = FIntVector4(Index == 0 ? 1 : 0, Index == 1 ? 1 : 0, Index == 2 ? 1 : 0, Index == 3 ? 1 : 0);
	}
};

UCLASS()
class SLIMECOMPUTESHADERDECLARATION_API USlimeAgentBlueprint : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="SlimeAgent|Methods")
	static void SetSpeciesIndex(UPARAM(ref) FSlimeAgent& SlimeAgent, int Index)
	{
		SlimeAgent.SetSpeciesIndex(Index);
	}
};
