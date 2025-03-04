// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "SaveGameInterface.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/InteractableInterface.h"
#include "AuraCheckpoint.generated.h"

UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart, public ISaveGameInterface, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
 	TObjectPtr<class USphereComponent> CheckpointSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
 	TObjectPtr<USceneComponent> TargetDestinationComponent;
	
protected:
	
	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Checkpoint")
	bool bCheckpointReached = false;
	
public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	//~ Begin of ISaveGameInterface
	virtual bool ShouldLoadTransform_Implementation() const override {return false;}
	virtual void LoadedFromSaveGame_Implementation() override;
	//~ End of ISaveGameInterface

	//~ Begin of IInteractableInterface
	virtual void HighlightActor_Implementation() override;
	virtual void UnhighlightActor_Implementation() override;
	virtual void GetMoveToDestination_Implementation(FVector& OutDestination) const override;
	//~ End of IInteractableInterface

	UFUNCTION(BlueprintCallable)
	void SetCheckpointReached(bool bNewValue);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);

	virtual void CheckpointReached();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CheckpointReached"))
	void ReceivedCheckpointReached();
};
