// Fill out your copyright notice in the Description page of Project Settings.


#include "TestObject.h"

void UTestObject::CallTimer()
{
	FTimerHandle tHandle;
	const float Delay = 1.0f;
	GetWorld()->GetTimerManager().SetTimer(tHandle, this, &UTestObject::TestTimer, Delay, false);
}

void UTestObject::TestTimer()
{
	GEngine->AddOnScreenDebugMessage(1,2,FColor::Red, "Hello World");
}

void UTestObject::Tick(float DeltaTime)
{
	TestCounter += DeltaTime;

	GEngine->AddOnScreenDebugMessage(0,0, FColor::Green, FString::SanitizeFloat(TestCounter));
}

TStatId UTestObject::GetStatId() const
{
	return TStatId();
}

bool UTestObject::IsTickable() const
{
	return true;
}

bool UTestObject::IsTickableInEditor() const
{
	return false;
}

bool UTestObject::IsTickableWhenPaused() const
{
	return false;
}

UWorld* UTestObject::GetWorld() const
{
	return GetOuter()->GetWorld();
}
