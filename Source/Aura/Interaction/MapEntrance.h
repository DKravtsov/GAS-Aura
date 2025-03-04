// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "Engine/StaticMeshActor.h"
#include "MapEntrance.generated.h"

UCLASS()
class AURA_API AMapEntrance : public AStaticMeshActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> TargetDestinationComponent;

public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

public:
	AMapEntrance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin of IInteractableInterface
	virtual void HighlightActor_Implementation() override;
	virtual void UnhighlightActor_Implementation() override;
	virtual void GetMoveToDestination_Implementation(FVector& OutDestination) const override;
	//~ End of IInteractableInterface

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	virtual void CheckpointReached();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CheckpointReached"))
	void ReceivedCheckpointReached();
};
