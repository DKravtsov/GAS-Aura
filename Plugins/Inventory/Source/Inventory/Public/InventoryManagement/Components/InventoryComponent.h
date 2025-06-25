// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/InventoryFastArray.h"
#include "InventoryComponent.generated.h"

class UInventoryItemComponent;
struct FInventorySlotAvailabilityResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChangeSignature, UInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryHasNoRoomSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChangedSignature, const FInventorySlotAvailabilityResult&, Result);

class UInventoryWidgetBase;

UCLASS(MinimalAPI, ClassGroup=(Inventory), Blueprintable, Abstract, meta=(BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	FInventoryItemChangeSignature OnItemAdded;
	FInventoryItemChangeSignature OnItemRemoved;
	FInventoryHasNoRoomSignature OnNoRoomInInventory;
	FStackChangedSignature OnStackChanged;

private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftClassPtr<UInventoryWidgetBase> InventoryMenuClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryWidgetBase> InventoryMenu;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMin = -85.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMax = 85.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMin = 10.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMax = 50.f;

	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<class UInventoryPlayerControllerComponent> InventoryController;

	UPROPERTY(Replicated)
	FInventoryFastArray InventoryList;

	bool bInventoryMenuOpen = false;
	
public:
	INVENTORY_API UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API void ToggleInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintAuthorityOnly)
	INVENTORY_API void TryAddItem(UInventoryItemComponent* ItemComponent);

	void AddRepSubObj(UObject* SubObj);

	INVENTORY_API void DropItem(UInventoryItem* Item, int32 StackCount);

protected:
	INVENTORY_API virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddNewItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStacksToItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(BlueprintNativeEvent)
	void GetDroppedItemSpawnLocationAndRotation(const FGameplayTag& ItemType, FVector& SpawnLocation, FRotator& SpawnRotation);
	virtual void GetDroppedItemSpawnLocationAndRotation_Implementation(const FGameplayTag& ItemType, FVector& SpawnLocation, FRotator& SpawnRotation);
	
private:

	void ConstructInventory();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	void SpawnDroppedItem(UInventoryItem* Item, int32 StackCount);
};
