// Copyright Epic Games, Inc. All Rights Reserved.

#include "NeuroPlayerController.h"

ANeuroPlayerController::ANeuroPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
