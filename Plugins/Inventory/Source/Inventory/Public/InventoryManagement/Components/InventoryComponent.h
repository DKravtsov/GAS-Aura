// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryGridTypes.h"
#include "Components/ActorComponent.h"
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
struct FInventoryItemProxy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftObjectPtr<UInventoryItemData> InventoryItem;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bOverrideCount"))
	FIntPoint MinMaxAmount {1,1};

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bShouldEquip"), DisplayName="Equip to slot")
	EInventoryEquipmentSlot EquipmentSlot = EInventoryEquipmentSlot::Invalid;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(InlineEditConditionToggle))
	uint8 bOverrideCount:1 = false;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(InlineEditConditionToggle))
	uint8 bShouldEquip:1 = false;

};

USTRUCT(BlueprintType)
struct FInventoryStartupEquipmentData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TWeakObjectPtr<UInventoryItem> InventoryItem;

	UPROPERTY()
	EInventoryEquipmentSlot Slot = EInventoryEquipmentSlot::Invalid;
};

USTRUCT(BlueprintType)
struct FStorageSetupDataProxy
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(ExcludeBaseStruct))
	TInstancedStruct<FInventoryStorageSetupData> SetupData;

	// Property to trigger the replication
	UPROPERTY()
	int Trigger = 0;

	const FInventoryStorageSetupData* GetData() const;
	TSubclassOf<UInventoryStorage> GetStorageClass() const;

	void MakeDirty() {++Trigger;}
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

	UPROPERTY(EditAnywhere, Category="Inventory", ReplicatedUsing=OnRep_StorageSetupData)
	FStorageSetupDataProxy InventoryStorageSetupData;

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

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<FInventoryEquipmentSlot> EquipmentSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventoryItemProxy> StartupInventoryItems;


	TArray<FInventoryStartupEquipmentData> StartupEquipment;

	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<class UInventoryPlayerControllerComponent> InventoryController;

	// List of WHAT is stored in the inventory
	UPROPERTY(Replicated)
	FInventoryFastArray InventoryList;

	uint8 bInventoryMenuOpen:1 = false;

	uint8 bStartupItemsInitialized:1 = false;
	uint8 bStartupItemsEquipped:1 = false;
	
public:
	INVENTORY_API UInventoryComponent();

	UInventoryWidgetBase* GetInventoryMenu() const { return InventoryMenu; }
	UInventoryStorage* GetInventoryStorage() const { return InventoryStorage; }

	const TArray<FInventoryEquipmentSlot>& GetEquipmentSlots() const {return EquipmentSlots;}

	const FInventoryEquipmentSlot* GetEquipmentSlot(EInventoryEquipmentSlot SlotId) const;
	const FInventoryEquipmentSlot* FindEquipmentSlotByEquippedItem(const UInventoryItem* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API void ToggleInventoryMenu();

	bool IsMenuOpen() const { return bInventoryMenuOpen; }

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintAuthorityOnly)
	INVENTORY_API void TryAddItem(UInventoryItemComponent* ItemComponent);

	void AddRepSubObj(UObject* SubObj);

	INVENTORY_API void DropItem(UInventoryItem* Item, int32 StackCount);
	INVENTORY_API void ConsumeItem(UInventoryItem* Item, int32 StackCount = 1);

	INVENTORY_API void EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId);

	TArray<FInventoryStartupEquipmentData> GetEquipStartupItems() const {return StartupEquipment;}
	bool TryEquipItem(UInventoryItem* ItemToEquip, EInventoryEquipmentSlot SlotId);

	void ReceivedStartupItems();
	void ReceivedStartupItemsEquipped() { bStartupItemsEquipped = true; }

	bool AreStartupItemsInitialized() const { return bStartupItemsInitialized; }
	bool AreStartupItemsEquipped() const { return bStartupItemsEquipped; }

	UFUNCTION()
	virtual void OnRep_InventoryStorage();

	void ReceivedStorageIsReady();

//#if UE_WITH_CHEAT_MANAGER
	INVENTORY_API void DebugPrintStorage() const;
//#endif//UE_WITH_CHEAT_MANAGER

protected:
	INVENTORY_API void CreateInventoryStorage();

	virtual void BeginPlay() override;

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

	FInventoryEquipmentSlot* FindEquipmentSlotByEquippedItemMutable(const UInventoryItem* Item);
	FInventoryEquipmentSlot* FindSuitableEquippedGridSlot(const FGameplayTag& ItemEquipmentTypeTag, bool bOnlyEmpty = true);
	FInventoryEquipmentSlot* GetEquipmentSlotMutable(EInventoryEquipmentSlot SlotId);

	EInventoryEquipmentSlot GetValidEquipSlotId(const FInventoryItemProxy& Item, const FInventoryItemManifest& ItemManifest);

};
