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

UENUM(BlueprintType)
enum class EDataFormat : uint8
{
	Absolute,
	Relative
};

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
	UFUNCTION(BlueprintCallable)
	int GetBrainFlowSize();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true))
	FString MacAddress = FString(TEXT("F4:0E:11:75:77:8F"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true, ToolTip="Absolute will give values in the range {-1,1}. Relative will give values in the range {0,1}."))
	EDataFormat BrainFlowDataFormat = EDataFormat::Absolute;

	const int BoardId = static_cast<int>(BoardIds::ENOPHONE_BOARD);
	TSharedPtr<BoardShim> BoardPtr;
	bool BoardRunning = false;
	int BufferSize = 500;

	std::vector<int> EegChannels;
	TArray<float> CachedBrainFlowData;
	TArray<float> RawCachedBrainFlowData;
	bool LatestCachedBrainFlowData = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true, ClampMin=0.001f, UIMin=0.001f, ToolTip="The value used to bring the BrainFlow data to a max of 1."))
	float MaxBrainFlowValue = 200.0f;

	void CreateBoard();
	void ConnectBoard();
	void StopBoard();
};
