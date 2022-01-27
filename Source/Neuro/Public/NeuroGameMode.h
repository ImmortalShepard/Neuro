#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BrainFlowPlugin.h"
#include "NeuroGameMode.generated.h"

UENUM(BlueprintType)
enum class EBrainFlowFormat : uint8
{
	Absolute,
	AbsolutePositive,
	AbsoluteAbsolute,
	Relative
};

UENUM(BlueprintType)
enum class EBrainFlowProcessing : uint8
{
	Current,
	Running
};

UCLASS()
class NEURO_API ANeuroGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ANeuroGameMode();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	bool GetBrainFlowData(TArray<float>& BrainFlowData);
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	bool GetRawBrainFlowData(TArray<float>& RawBrainFlowData);
	UFUNCTION(BlueprintCallable)
	int GetBrainFlowSize();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true))
	FString MacAddress = FString(TEXT("F4:0E:11:75:77:8F"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true))
	EBrainFlowProcessing BrainFlowProcessing = EBrainFlowProcessing::Current;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true))
	EBrainFlowFormat BrainFlowFormat = EBrainFlowFormat::Relative;

	const int BoardId = static_cast<int>(BoardIds::ENOPHONE_BOARD);
	TSharedPtr<BoardShim> BoardPtr;
	bool BoardRunning = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BrainFlow", meta=(AllowPrivateAccess=true))
	int BufferSize = 10;

	std::vector<int> EegChannels;
	TArray<float> CachedBrainFlowData;
	TArray<float> RawCachedBrainFlowData;
	bool LatestCachedBrainFlowData = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BrainFlow", meta=(AllowPrivateAccess=true, ClampMin=0.001f, UIMin=0.001f))
	float MaxBrainFlowValue = 200.0f;

	void CreateBoard();
	void ConnectBoard();
	void StopBoard();

	void LatestBrainFlowData();
	void AbsoluteLatest(BrainFlowArray<double, 2>& Data);
	void AbsolutePositiveLatest(BrainFlowArray<double, 2>& Data);
	void AbsoluteAbsoluteLatest(BrainFlowArray<double, 2>& Data);
	void RelativeLatest(BrainFlowArray<double, 2>& Data);
};
