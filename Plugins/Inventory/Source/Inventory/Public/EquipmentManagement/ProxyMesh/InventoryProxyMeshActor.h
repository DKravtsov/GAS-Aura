// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryProxyMeshActor.generated.h"

class UInventoryEquipmentComponent;

UCLASS(MinimalAPI, Blueprintable, Abstract)
class AInventoryProxyMeshActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInventoryEquipmentComponent> EquipmentComponent;

	// The proxy mesh we see in the Inventory menu
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	// The pointer to the player-controlled Character's mesh
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;

	FTimerHandle TimerHandle;

public:
	AInventoryProxyMeshActor();

protected:
	virtual void BeginPlay() override;

private:

	void DelayedInitializeOwner();
	void DelayedInitialize();
};
