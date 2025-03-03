// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "SaveGameInterface.h"
#include "GameFramework/PlayerStart.h"
#include "AuraCheckpoint.generated.h"

UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart, public ISaveGameInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
 	TObjectPtr<class USphereComponent> CheckpointSphere;
	
protected:
	
	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Checkpoint")
	bool bCheckpointReached = false;
	
public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	//~ Begin of ISaveGameInterface
	virtual bool ShouldLoadTransform_Implementation() const override {return false;}
	virtual void LoadedFromSaveGame_Implementation() override;
	//~ End of ISaveGameInterface

	UFUNCTION(BlueprintCallable)
	void SetCheckpointReached(bool bNewValue);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintImplementableEvent, meta = (DeprecatedFunction))
	void CheckpointReached(AActor* Actor);

	virtual void CheckpointReached();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CheckpointReached"))
	void ReceivedCheckpointReached();
};
