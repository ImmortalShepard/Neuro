#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BrainFlowPlugin.h"
#include "BrainFlowActor.generated.h"

//USTRUCT(BlueprintType)
//struct NEURO_API FBrainFlowData
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float A2;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float A1;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float C4;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float C3;
//};

UCLASS()
class NEURO_API ABrainFlowActor : public AActor
{
	GENERATED_BODY()
	
public:
	ABrainFlowActor();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	bool GetBrainFlowData(TArray<float>& BrainFlowData);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true))
	FString MacAddress = FString(TEXT("F4:0E:11:75:77:8F"));

	const int BoardId = static_cast<int>(BoardIds::ENOPHONE_BOARD);
	TSharedPtr<BoardShim> BoardPtr;
	bool BoardRunning = false;
	int BufferSize = 500;

	std::vector<int> EegChannels;
	TArray<float> CachedBrainFlowData;
	bool LatestCachedBrainFlowData = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true, ClampMin=0, UIMin=0))
	float MaxBrainFlowValue = 200.0f;

	void CreateBoard();
	void ConnectBoard();
	void StopBoard();
};
