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
	bool bActivated = false;
	
public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(AActor* Actor);
};
