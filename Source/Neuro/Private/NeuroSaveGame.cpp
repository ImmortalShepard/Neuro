#include "NeuroSaveGame.h"

FString UNeuroSaveGame::SlotName = TEXT("NeuroSave");

void UNeuroSaveGame::AsyncSave(UNeuroSaveGame* SaveGame, FAsyncSaveGameToSlotDelegate& SaveDelegate)
{
	if (SaveGame)
	{
		UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SlotName, 0, SaveDelegate);
	}
}

bool UNeuroSaveGame::SyncSave(UNeuroSaveGame* SaveGame)
{
	if (SaveGame)
	{
		return UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);
	}
	return false;
}

void UNeuroSaveGame::AsyncLoad(FAsyncLoadGameFromSlotDelegate& LoadDelegate)
{
	UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, LoadDelegate);
}

UNeuroSaveGame* UNeuroSaveGame::SyncLoad()
{
	return Cast<UNeuroSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
}
