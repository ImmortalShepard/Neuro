#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "NeuroSaveGame.generated.h"

UCLASS()
class NEURO_API UNeuroSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="BrainFlow")
	FString MacAddress = FString(TEXT("F4:0E:11:75:77:8F"));

	static void AsyncSave(UNeuroSaveGame* SaveGame, FAsyncSaveGameToSlotDelegate& SaveDelegate);
	static bool SyncSave(UNeuroSaveGame* SaveGame);
	static void AsyncLoad(FAsyncLoadGameFromSlotDelegate& LoadDelegate);
	static UNeuroSaveGame* SyncLoad();

private:
	static FString SlotName;
};
