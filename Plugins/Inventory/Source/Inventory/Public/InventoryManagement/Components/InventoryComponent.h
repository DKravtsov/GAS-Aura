// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryGridTypes.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/InventoryEquipmentSlotFastArray.h"
#include "InventoryManagement/FastArray/InventoryFastArray.h"
#include "InventoryComponent.generated.h"

class UInventoryStoreComponent;
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

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemDelegate, UInventoryItem* /*Item*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FInventoryItemWithIndexDelegate, UInventoryItem* /*Item*/, int32 /*Index*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FInventoryItemWithIndexAndStackCountDelegate, UInventoryItem* /*Item*/, int32 /*Index*/, int32 /*StackCount*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemAvailabilityResultDelegate, const FInventorySlotAvailabilityResult& /*Result*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FInventoryHoverItemUpdatedDelegate, UInventoryItem* /*Item*/, bool /*bStackable*/, int32 /*StackCount*/, int32 /*PreviousIndex*/);
DECLARE_MULTICAST_DELEGATE(FInventoryHoverItemResetDelegate);

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
	FItemEquipStatusChangedSignature OnItemEquipStatusChanged;

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FInventoryMenuVisibilityChangedSugnature OnInventoryMenuOpened;
	
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FInventoryMenuVisibilityChangedSugnature OnInventoryMenuClosed;

	FInventoryHoverItemUpdatedDelegate OnHoverItemUpdated;
	FInventoryHoverItemResetDelegate OnHoverItemReset;

private:

	// Inventory Setup Data
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSoftObjectPtr<class UInventorySetupData> InventorySetupData;

	// Responsible for HOW the inventory is stored
	UPROPERTY(Replicated)
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

	// Temp array to sync between client and server.
	// todo: probably can be replaced/removed if equipped items are synced through fast array or RPC
	TArray<FInventoryStartupEquipmentData> StartupEquipment;

	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<class UInventoryPlayerControllerComponent> InventoryController;

	// List of WHAT is stored in the inventory
	UPROPERTY(Replicated)
	FInventoryFastArray InventoryList;

	// List of equipment slots
	UPROPERTY(Replicated)
	FInventoryEquipmentSlotFastArray EquipmentSlots;

	struct FHoverItemProxy
	{
		TWeakObjectPtr<UInventoryItem> InventoryItem;
		int32 PreviousIndex = INDEX_NONE; // index on grid where this item was taken
		int32 StackCount = 0;
		bool bStackable = false; // a shortcut whether this item is stackable or not
	};

	// This is kinda "selected" item - item that a player is manipulating at the moment (e.g. is moving to another slot)
	// This is needed on the server to track what's happened on the client.
	TOptional<FHoverItemProxy> HoverItemProxy;

	mutable TWeakObjectPtr<UInventoryItem> CoinsItem;

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

	bool HasHoverItem() const {return HoverItemProxy.IsSet();}

	TArray<FInventoryEquipmentSlot> GetEquipmentSlotsCopy() const {return EquipmentSlots.GetAllItems();}

	const FInventoryEquipmentSlot* GetEquipmentSlot(EInventoryEquipmentSlot SlotId) const;
	const FInventoryEquipmentSlot* FindEquipmentSlotByEquippedItem(const UInventoryItem* Item) const;
	// Be aware, that this also marks the entry dirty for replication
	FInventoryEquipmentSlot* GetEquipmentSlotMutable(EInventoryEquipmentSlot SlotId);

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintAuthorityOnly)
	INVENTORY_API void TryAddItem(UInventoryItemComponent* ItemComponent);

	INVENTORY_API void DropItem(UInventoryItem* Item, int32 StackCount);
	INVENTORY_API void ConsumeItem(UInventoryItem* Item, int32 GridIndex, int32 StackCount = 1);

	INVENTORY_API void EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);

	TArray<FInventoryStartupEquipmentData> GetEquipStartupItems() const {return StartupEquipment;}
	void TryEquipItem(UInventoryItem* ItemToEquip, EInventoryEquipmentSlot SlotId);

	void ReceivedStartupItems();
	void ReceivedStartupItemsEquipped() { bStartupItemsEquipped = true; }
	bool AreStartupItemsInitialized() const { return bStartupItemsInitialized; }
	bool AreStartupItemsEquipped() const { return bStartupItemsEquipped; }

//#if UE_WITH_CHEAT_MANAGER
	INVENTORY_API void DebugPrintStorage() const;
//#endif//UE_WITH_CHEAT_MANAGER

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FillInStacksOrConsumeHover(UInventoryItem* Item, int32 GridIndex);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwapStackCountWithHoverItem(UInventoryItem* Item, int32 GridIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SelectItem(UInventoryItem* Item, int32 GridIndex, int32 PrevIndex);

	// User is trying to Equip/Unequip item. If already has selected item - swap it with the item in the slot 
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SelectItemInSlot(EInventoryEquipmentSlot SlotId);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipSelectedItem(EInventoryEquipmentSlot SlotId);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PutSelectedItemToStorage();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PutSelectedItemToStorageAtIndex(const int32 TargetIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropSelectedItemOff();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SplitStackToHoverItem(UInventoryItem* Item, int32 GridIndex, int32 SplitAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwapSelectedWitItem(UInventoryItem* ItemOnGrid, int32 GridIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellSelectedItem(UInventoryStoreComponent* Store);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(UInventoryStoreComponent* Store, UInventoryItem* ItemToSell, int32 StackCount);

	// returns the number of coins
	int32 GetWealth() const;

protected:
	INVENTORY_API void CreateInventoryStorage();

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryAddItem(UInventoryItemComponent* ItemComponent);

	void AddNewItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);
	void AddStacksToItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ConsumeItem(UInventoryItem* Item, int32 GridIndex, int32 StackCount);

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

	bool RemoveItemFromInventory(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_RequestStartupEquipment();

	void ExchangeItemsWithOtherInventory(UInventoryComponent* OtherInventory,
		const FInventoryItemManifest& ItemManifestA, int32 StackCountA,
		const FInventoryItemManifest& ItemManifestB, int32 StackCountB);

private:
	void SetOwnerInternal();

	void ConstructInventoryMenu();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	void BroadcastEquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);
	void SpawnDroppedItem(UInventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStartupItems();

	// returns a newly created item or null if the item already in the inventory or failed
	UInventoryItem* TryAddItem(const FInventoryItemManifest& ItemManifest, int32 StackCount);
	UInventoryItem* AddNewItem(const FInventoryItemManifest& ItemManifest, int32 StackCount);
	void AddStacksToItem(const FInventoryItemManifest& ItemManifest, int32 StackCount);

	UFUNCTION(Client, Reliable)
	void Client_EquipStartupInventory(const TArray<FInventoryStartupEquipmentData>& StartupEquipmentData);

	EInventoryEquipmentSlot FindSuitableEquippedGridSlot(const FGameplayTag& ItemEquipmentTypeTag, bool bOnlyEmpty = true) const;


	EInventoryEquipmentSlot GetValidEquipSlotId(EInventoryEquipmentSlot DesiredSlotId, const FInventoryItemManifest& ItemManifest);

	void AddItemAtIndex(UInventoryItem* Item, int32 Index, bool bStackable, int32 StackCount);
	void RemoveItemFromStorage(UInventoryItem* Item, int32 GridIndex);

	void NotifyHoverItemUpdated();
	void ClearSelectedItem();

	UFUNCTION(Client, Reliable)
	void Client_ReceivedHoverItemUpdated(UInventoryItem* InventoryItem, bool bStackable, int32 StackCount, int32 PreviousIndex);

	UFUNCTION(Client, Reliable)
	void Client_ReceivedHoverItemReset();

};
