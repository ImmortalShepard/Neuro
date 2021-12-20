// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestObject.generated.h"

/**
 * 
 */
UCLASS()
class NEURO_API UTestObject : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Test)
	void CallTimer();

	void TestTimer();

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;

	UWorld* GetWorld() const override;

	float TestCounter;
};
