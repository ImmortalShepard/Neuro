#pragma once

#include "CoreMinimal.h"

#include "SlimeAgent.h"
#include "SlimeComputeShaderDeclarationModule.h"
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

	UFUNCTION(BlueprintImplementableEvent)
	FSlimeAgent CustomSpawn(int ListIndex);
	UFUNCTION(BlueprintCallable)
	FSlimeAgent& AssignSpeciesByListIndex(UPARAM(ref) FSlimeAgent& SlimeAgent, int ListIndex) const;
	UFUNCTION(BlueprintCallable)
	void UpdateSlimeSim(const TArray<FSlimeSpeciesSettings>& SpeciesSettings, const FBoundsSettings& BoundsSettings);
	UFUNCTION(BlueprintCallable)
	void UpdateSlimeSimSpecies(const TArray<FSlimeSpeciesSettings>& SpeciesSettings);
	UFUNCTION(BlueprintCallable)
	void UpdateSlimeSimBounds(const FBoundsSettings& BoundsSettings);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SpawnRandom(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnPoint(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnInwardCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnOutwardCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnRandomCircle(TResourceArray<FSlimeAgent>& SlimeAgents) const;
	void SpawnCustom(TResourceArray<FSlimeAgent>& SlimeAgents);
	void AssignSpecies(FSlimeAgent& SlimeAgent, int ListIndex) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=SlimeSim, meta=(AllowPrivateAccess=true))
	UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=SlimeSim, meta=(AllowPrivateAccess=true))
	FIntPoint Resolution;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=SlimeSim, meta=(AllowPrivateAccess=true))
	USlimeSettings* SlimeSettings;
	FSlimeComputeShaderParameters DrawParameters;
};
