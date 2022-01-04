#pragma once

#include "CoreMinimal.h"

#include "NeuroSaveGame.h"
#include "Engine/GameInstance.h"
#include "NeuroGameInstance.generated.h"

UCLASS()
class NEURO_API UNeuroGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FString GetMacAddress();
	void SetMacAddress(FString NewMacAddress);

	virtual void Init() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BrainFlow", meta=(AllowPrivateAccess=true))
	UNeuroSaveGame* NeuroSaveGame;
};
