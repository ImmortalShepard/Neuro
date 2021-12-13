// Copyright Epic Games, Inc. All Rights Reserved.

#include "NeuroGameMode.h"
#include "NeuroPlayerController.h"
#include "NeuroPawn.h"

ANeuroGameMode::ANeuroGameMode()
{
	// no pawn by default
	DefaultPawnClass = ANeuroPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = ANeuroPlayerController::StaticClass();
}
