#include "NeuroGameInstance.h"

FString UNeuroGameInstance::GetMacAddress()
{
	if (NeuroSaveGame == nullptr)
	{
		NeuroSaveGame = Cast<UNeuroSaveGame>(UGameplayStatics::CreateSaveGameObject(UNeuroSaveGame::StaticClass()));
	}
	return NeuroSaveGame->MacAddress;
}

void UNeuroGameInstance::SetMacAddress(FString NewMacAddress)
{
	NeuroSaveGame->MacAddress = NewMacAddress;
	FAsyncSaveGameToSlotDelegate saveDelegate;
	UNeuroSaveGame::AsyncSave(NeuroSaveGame, saveDelegate);
}

void UNeuroGameInstance::Init()
{
	NeuroSaveGame = UNeuroSaveGame::SyncLoad();
}
