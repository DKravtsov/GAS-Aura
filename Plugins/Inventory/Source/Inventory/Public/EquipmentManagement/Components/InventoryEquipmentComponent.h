// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryEquipmentComponent.generated.h"

class UInventoryComponent;
class UInventoryItem;

UCLASS(MinimalAPI, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, Abstract)
class UInventoryEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

	TWeakObjectPtr<UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class AInventoryEquipActor>> EquippedActors;

public:

	UInventoryEquipmentComponent();

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnItemEquipped(UInventoryItem* EquippedItem);
	
	UFUNCTION()
	void OnItemUnequipped(UInventoryItem* UnequippedItem);

	void InitInventoryComponent();

	class AInventoryEquipActor* SpawnEquippedActor(struct FInventoryItemEquipmentFragment& EquipmentFragment, const struct FInventoryItemManifest& ItemManifest, USkeletalMeshComponent* ParentMesh);
};
