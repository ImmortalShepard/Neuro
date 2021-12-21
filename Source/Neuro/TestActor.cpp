// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor.h"

// Sets default values
ATestActor::ATestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();

	CreateBoard();
	ConnectBoard();
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestActor::BeginDestroy()
{
	if (BoardRunning)
	{
		BoardPtr->stop_stream();
		BoardPtr->release_session();
		BoardRunning = false;
	}
}

void ATestActor::CreateBoard()
{
	int boardId = static_cast<int>(BoardIds::ENOPHONE_BOARD);
	BrainFlowInputParams params;
	params.mac_address = TCHAR_TO_UTF8(*MacAddress);

	BoardPtr = MakeShared<BoardShim>(boardId, params);
}

void ATestActor::ConnectBoard()
{
	try
	{
		BoardPtr->prepare_session();
		BoardPtr->start_stream();
		BoardRunning = true;
	}
	catch (const BrainFlowException& Err)
	{
		UE_LOG(LogTemp, Warning, TEXT("BrainFlow: %hs"), Err.what());
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("BrainFlow: %hs"), Err.what()));

		if (BoardPtr->is_prepared())
		{
			BoardPtr->release_session();
		}
	}
}
