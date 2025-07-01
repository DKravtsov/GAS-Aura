// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractableInterface.h"
#include "AuraPickupActor.generated.h"

UCLASS(Abstract, Blueprintable)
class AAuraPickupActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta=(AllowPrivateAccess=true))
	TObjectPtr<class UInventoryItemComponent> InventoryItemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess=true))
	TObjectPtr<UMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float RotationRate = 2.f;
	
public:

	static FName InventoryItemComponentName;
	static FName MeshComponentName;
	
	// Sets default values for this actor's properties
	AAuraPickupActor(const FObjectInitializer& ObjectInitializer=FObjectInitializer::Get());

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//~ Begin of IInteractableInterface
	virtual void GetMoveToDestination_Implementation(FVector& OutDestination) const override;
	//~ End of IInteractableInterface

};
