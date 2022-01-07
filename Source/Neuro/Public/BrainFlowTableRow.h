#pragma once

#include "Engine/DataTable.h"
#include "BrainFlowTableRow.generated.h"

USTRUCT(BlueprintType)
struct FBrainFlowTableRow : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float A2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float A1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float C4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float C3;
};