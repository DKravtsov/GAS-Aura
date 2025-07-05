// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/InventoryFastArray.h"
#include "InventoryComponent.generated.h"

class UInventoryItemData;
class UInventoryItemComponent;
struct FInventorySlotAvailabilityResult;
class UInventoryWidgetBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChangeSignature, UInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryHasNoRoomSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChangedSignature, const FInventorySlotAvailabilityResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquipStatusChangedSignature, UInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryMenuVisibilityChangedSugnature);

USTRUCT(BlueprintType)
struct FInventoryItemProxy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftObjectPtr<UInventoryItemData> InventoryItem;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bOverrideCount"))
	FIntPoint MinMaxAmount {1,1};

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(Categories="GameItems.Equipment"))
	FGameplayTag ShouldEquipToSlot;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(InlineEditConditionToggle))
	uint8 bOverrideCount:1 = false;

};

UCLASS(MinimalAPI, ClassGroup=(Inventory), Blueprintable, Abstract, Within=PlayerController, meta=(BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	FInventoryItemChangeSignature OnItemAdded;
	FInventoryItemChangeSignature OnItemRemoved;
	FInventoryHasNoRoomSignature OnNoRoomInInventory;
	FStackChangedSignature OnStackChanged;
	FItemEquipStatusChangedSignature OnItemEquipped;
	FItemEquipStatusChangedSignature OnItemUnequipped;

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FInventoryMenuVisibilityChangedSugnature OnInventoryMenuOpened;
	
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FInventoryMenuVisibilityChangedSugnature OnInventoryMenuClosed;

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

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventoryItemProxy> StartupInventoryItems;

	TArray<TPair<TWeakObjectPtr<UInventoryItem>, FGameplayTag>> StartupEquipment;

	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<class UInventoryPlayerControllerComponent> InventoryController;

	UPROPERTY(Replicated)
	FInventoryFastArray InventoryList;

	uint8 bInventoryMenuOpen:1 = false;

	uint8 bStartupItemsInitialized:1 = false;
	uint8 bStartupItemsEquipped:1 = false;
	
public:
	INVENTORY_API UInventoryComponent();

	UInventoryWidgetBase* GetInventoryMenu() const { return InventoryMenu; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API void ToggleInventoryMenu();

	bool IsMenuOpen() const { return bInventoryMenuOpen; }

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintAuthorityOnly)
	INVENTORY_API void TryAddItem(UInventoryItemComponent* ItemComponent);

	void AddRepSubObj(UObject* SubObj);

	INVENTORY_API void DropItem(UInventoryItem* Item, int32 StackCount);
	INVENTORY_API void ConsumeItem(UInventoryItem* Item, int32 StackCount = 1);

	INVENTORY_API void EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip);

	TArray<TPair<TWeakObjectPtr<UInventoryItem>, FGameplayTag>> GetEquipStartupItems() const {return StartupEquipment;}
	bool TryEquipItem(UInventoryItem* ItemToEquip, const FGameplayTag& EquipmentTypeTag) const;

	void ReceivedStartupItems();
	void ReceivedStartupItemsEquipped() { bStartupItemsEquipped = true; }

	bool AreStartupItemsInitialized() const { return bStartupItemsInitialized; }
	bool AreStartupItemsEquipped() const { return bStartupItemsEquipped; }

protected:
	INVENTORY_API virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddNewItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStacksToItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ConsumeItem(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip);
	
	UFUNCTION(BlueprintNativeEvent)
	void GetDroppedItemSpawnLocationAndRotation(const FGameplayTag& ItemType, FVector& SpawnLocation, FRotator& SpawnRotation);
	virtual void GetDroppedItemSpawnLocationAndRotation_Implementation(const FGameplayTag& ItemType, FVector& SpawnLocation, FRotator& SpawnRotation);
	
private:

	void ConstructInventory();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	void SpawnDroppedItem(UInventoryItem* Item, int32 StackCount);

	void /*Client*/TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag);
	FInventorySlotAvailabilityResult ServerCheckHasRoomForItem(const FInventoryItemManifest& ItemManifest, int32 StackCountOverride) const;
	void Server_TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStartupItems();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddNewStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStacksToItemAtStart(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION(Client, Reliable)
	void Client_ReceivedStartupInventory();

};
