// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BrainFlowPlugin.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestActor.generated.h"

UCLASS()
class NEURO_API ATestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrivateTestActor", meta = (AllowPrivateAccess = "true"))
	FString MacAddress = FString(TEXT("F40E1175778F"));

	TSharedPtr<BoardShim> BoardPtr;
	bool BoardRunning = false;

	void CreateBoard();
	void ConnectBoard();
};
