#pragma once

#include "CoreMinimal.h"


#include "BrainFlowTableRow.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "BrainFlowTableActor.generated.h"

UCLASS()
class NEURO_API ABrainFlowTableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABrainFlowTableActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FBrainFlowTableRow GetBrainFlowData();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UDataTable* DataTable;
};
