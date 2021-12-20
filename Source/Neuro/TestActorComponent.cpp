// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActorComponent.h"
#include "BrainFlowPlugin.h"

// Sets default values for this component's properties
UTestActorComponent::UTestActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTestActorComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateBrainFlow();
	ConnectBrainFlow();
}


// Called every frame
void UTestActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTestActorComponent::CreateBrainFlow()
{
	BoardShim::enable_dev_board_logger();

	int boardId = static_cast<int>(BoardIds::ENOPHONE_BOARD);
	struct BrainFlowInputParams params;
	params.mac_address = "F40E1175778F";

	board = MakeUnique<BoardShim>(boardId, params);
}

void UTestActorComponent::ConnectBrainFlow()
{
	try
	{
		board->prepare_session();
		board->start_stream();
	}
	catch (const BrainFlowException &Err)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), Err.what());

		FVector test;

		BoardShim::log_message(static_cast<int>(LogLevels::LEVEL_ERROR), Err.what());
		if (board->is_prepared())
		{
			board->release_session();
		}
	}
}
