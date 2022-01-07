// Fill out your copyright notice in the Description page of Project Settings.


#include "BrainFlowTableActor.h"

// Sets default values
ABrainFlowTableActor::ABrainFlowTableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABrainFlowTableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABrainFlowTableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FBrainFlowTableRow ABrainFlowTableActor::GetBrainFlowData()
{
	int rowNumber = FMath::FloorToInt(GetWorld()->TimeSeconds) % DataTable->GetRowMap().Num();
	FBrainFlowTableRow* row = DataTable->FindRow<FBrainFlowTableRow>(FName(FString::FromInt(rowNumber)), TEXT("BrainFlow Table"));
	return *row;
}
