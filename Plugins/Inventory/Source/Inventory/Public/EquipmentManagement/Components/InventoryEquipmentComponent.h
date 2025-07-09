// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "InventoryEquipmentComponent.generated.h"

class UInventoryComponent;
class UInventoryItem;
class AInventoryEquipActor;

UCLASS(MinimalAPI, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, HideDropdown)
class UInventoryEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

	TWeakObjectPtr<UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UInventoryItem>, TObjectPtr<AInventoryEquipActor>> EquippedActors;

	bool bIsProxy = false;

public:

	UInventoryEquipmentComponent();

	void SetOwningSkeletalMesh(USkeletalMeshComponent* NewOwningSkeletalMesh);
	void SetIsProxy(bool bProxy) {bIsProxy = bProxy;}

	void InitializeOwner(APlayerController* PlayerController);

	AInventoryEquipActor* FindEquippedActorByEquipmentType(const FGameplayTag& EquipmentType) const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
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
	void RemoveEquippedActor(UInventoryItem* Item);

	void InitStartupEquipment();
	void WaitForStartupEquipmentReady();
};
