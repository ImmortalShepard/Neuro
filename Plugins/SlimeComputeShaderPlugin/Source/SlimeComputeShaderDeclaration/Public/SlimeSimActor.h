#pragma once

#include "CoreMinimal.h"

#include "SlimeAgent.h"
#include "SlimeSettings.h"
#include "GameFramework/Actor.h"
#include "SlimeSimActor.generated.h"

UCLASS()
class SLIMECOMPUTESHADERDECLARATION_API ASlimeSimActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeSimActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintImplementableEvent, meta=(AllowPrivateAccess=true))
	FSlimeAgent CustomSpawn(int Index);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SpawnRandom(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnPoint(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnInwardCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnRandomCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnCustom(TResourceArray<FSlimeAgent>& SlimeAgents);
	
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess=true))
	void AssignSpecies(FSlimeAgent& SlimeAgent, int Index) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=SlimeSim, meta=(AllowPrivateAccess=true))
	UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=SlimeSim, meta=(AllowPrivateAccess=true))
	FIntPoint Resolution;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=SlimeSim, meta=(AllowPrivateAccess=true))
	USlimeSettings* SlimeSettings;
};
