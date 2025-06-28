// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	TMap<FGameplayTag, TObjectPtr<class AInventoryEquipActor>> EquippedActors;

public:

	UInventoryEquipmentComponent();


	AInventoryEquipActor* FindEquippedActorByEquipmentType(const FGameplayTag& EquipmentType) const;
	
protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnItemEquipped(UInventoryItem* EquippedItem);
	
	UFUNCTION()
	void OnItemUnequipped(UInventoryItem* UnequippedItem);

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void InitInventoryComponent();
	void InitPlayerController();

	AInventoryEquipActor* SpawnEquippedActor(struct FInventoryItemEquipmentFragment& EquipmentFragment, const struct FInventoryItemManifest& ItemManifest, USkeletalMeshComponent* ParentMesh);
	void RemoveEquippedActorOfType(const FGameplayTag& EquipmentType);
};
