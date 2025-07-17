// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryGridTypes.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/InventoryEquipmentSlotFastArray.h"
#include "InventoryManagement/FastArray/InventoryFastArray.h"
#include "StructUtils/InstancedStruct.h"
#include "InventoryComponent.generated.h"

struct FInventoryStorageSetupData;
class UInventoryItemData;
class UInventoryItemComponent;
struct FInventorySlotAvailabilityResult;
class UInventoryWidgetBase;
class UInventoryStorage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChangeSignature, UInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryHasNoRoomSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChangedSignature, const FInventorySlotAvailabilityResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquipStatusChangedSignature, UInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryMenuVisibilityChangedSugnature);

USTRUCT(BlueprintType)
struct FInventoryStartupEquipmentData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TWeakObjectPtr<UInventoryItem> InventoryItem;

	UPROPERTY()
	EInventoryEquipmentSlot Slot = EInventoryEquipmentSlot::Invalid;
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

	// Inventory Setup Data
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSoftObjectPtr<class UInventorySetupData> InventorySetupData;

	// Responsible for HOW the inventory is stored
	UPROPERTY(ReplicatedUsing=OnRep_InventoryStorage)
	TObjectPtr<UInventoryStorage> InventoryStorage;
	
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

	// Temp array to sync between client and server. todo: probably can be replaced/removed if equipped items are synced through fast array
	TArray<FInventoryStartupEquipmentData> StartupEquipment;

	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<class UInventoryPlayerControllerComponent> InventoryController;

	// List of WHAT is stored in the inventory
	UPROPERTY(Replicated)
	FInventoryFastArray InventoryList;

	// List of equipment slots
	UPROPERTY(Replicated)
	FInventoryEquipmentSlotFastArray EquipmentSlots;

	uint8 bInventoryMenuOpen:1 = false;

	uint8 bStartupItemsInitialized:1 = false;
	uint8 bStartupItemsEquipped:1 = false;
	
public:
	INVENTORY_API UInventoryComponent();

	void AddRepSubObj(UObject* SubObj);

	UInventoryWidgetBase* GetInventoryMenu() const { return InventoryMenu; }
	UInventoryStorage* GetInventoryStorage() const { return InventoryStorage; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API void ToggleInventoryMenu();

	bool IsMenuOpen() const { return bInventoryMenuOpen; }

	TArray<FInventoryEquipmentSlot> GetEquipmentSlotsCopy() const {return EquipmentSlots.GetAllItems();}

	const FInventoryEquipmentSlot* GetEquipmentSlot(EInventoryEquipmentSlot SlotId) const;
	const FInventoryEquipmentSlot* FindEquipmentSlotByEquippedItem(const UInventoryItem* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintAuthorityOnly)
	INVENTORY_API void TryAddItem(UInventoryItemComponent* ItemComponent);

	INVENTORY_API void DropItem(UInventoryItem* Item, int32 StackCount);
	INVENTORY_API void ConsumeItem(UInventoryItem* Item, int32 StackCount = 1);

	INVENTORY_API void EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);

	TArray<FInventoryStartupEquipmentData> GetEquipStartupItems() const {return StartupEquipment;}
	bool TryEquipItem(UInventoryItem* ItemToEquip, EInventoryEquipmentSlot SlotId);

	void ReceivedStartupItems();
	void ReceivedStartupItemsEquipped() { bStartupItemsEquipped = true; }
	bool AreStartupItemsInitialized() const { return bStartupItemsInitialized; }
	bool AreStartupItemsEquipped() const { return bStartupItemsEquipped; }
	void ReceivedStorageIsReady();

//#if UE_WITH_CHEAT_MANAGER
	INVENTORY_API void DebugPrintStorage() const;
	TArray<UInventoryItem*> GetAllInventoryItems() const;
//#endif//UE_WITH_CHEAT_MANAGER

protected:
	INVENTORY_API void CreateInventoryStorage();

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryAddItem(UInventoryItemComponent* ItemComponent);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddNewItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStacksToItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ConsumeItem(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);

	UFUNCTION(Client, Reliable)
	void Client_EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);

	UFUNCTION(Client, Reliable)
	void Client_RejectEquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);
	
	UFUNCTION(BlueprintNativeEvent)
	void GetDroppedItemSpawnLocationAndRotation(const FGameplayTag& ItemType, FVector& SpawnLocation, FRotator& SpawnRotation);
	virtual void GetDroppedItemSpawnLocationAndRotation_Implementation(const FGameplayTag& ItemType, FVector& SpawnLocation, FRotator& SpawnRotation);

	UFUNCTION(Server, Reliable)
	void Server_RequestStartupEquipment();

	UFUNCTION()
	virtual void OnRep_StorageSetupData();

	UFUNCTION()
	virtual void OnRep_EquipmentSlots();

	UFUNCTION()
	virtual void OnRep_InventoryStorage();

private:
	void SetOwnerInternal();

	void ConstructInventoryMenu();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	void BroadcastEquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);
	void SpawnDroppedItem(UInventoryItem* Item, int32 StackCount);

//	void /*Client*/TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot);
	FInventorySlotAvailabilityResult ServerCheckHasRoomForItem(const FInventoryItemManifest& ItemManifest, int32 StackCountOverride) const;
	void Server_TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStartupItems();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddNewStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStacksToItemAtStart(const FInventoryItemManifest& ItemManifest, int32 StackCount);

	UFUNCTION(Client, Reliable)
	void Client_ReceivedStartupInventory(const TArray<FInventoryStartupEquipmentData>& StartupEquipmentData);

	EInventoryEquipmentSlot FindSuitableEquippedGridSlot(const FGameplayTag& ItemEquipmentTypeTag, bool bOnlyEmpty = true) const;

	// Be aware, that this also marks the entry dirty for replication
	FInventoryEquipmentSlot* GetEquipmentSlotMutable(EInventoryEquipmentSlot SlotId);

	EInventoryEquipmentSlot GetValidEquipSlotId(EInventoryEquipmentSlot DesiredSlotId, const FInventoryItemManifest& ItemManifest);

};
