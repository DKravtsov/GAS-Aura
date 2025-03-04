// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Game/SaveGameInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemySpawnVolume : public ATriggerBox, public ISaveGameInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class AAuraEnemySpawnPoint*> SpawnPoints;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	bool bReached = false;

public:
	AAuraEnemySpawnVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~ Begin of ISaveGameInterface
	virtual void LoadedFromSaveGame_Implementation() override;
	//~ End of ISaveGameInterface

protected:
	

	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult);

	virtual void CheckpointReached();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CheckpointReached"))
	void ReceivedCheckpointReached();
};
