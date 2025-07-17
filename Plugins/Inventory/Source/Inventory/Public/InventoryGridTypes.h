// Copyright 4sandwiches

#pragma once

#include "NativeGameplayTags.h"
#include "InventoryGridTypes.generated.h"


namespace InventoryTags
{
	/**  Item Categories **/

	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_ItemCategory);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_ItemCategory_Equipment);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_ItemCategory_Consumable);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_ItemCategory_Crafting);
	
	/**  Equipment Slots **/

	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_EquipmentSlots);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_EquipmentSlots_Armour);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_EquipmentSlots_Weapon);
	
}

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemGridChangedDelegate, const struct FInventorySlotAvailabilityResult& /*Result*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemChangedDelegate, class UInventoryItem* /*Item*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryGridSlotsUpdatedSignature, const TArrayView<int32>& /*GridIndexArray*/);

USTRUCT()
struct FInventorySlotAvailability
{
	GENERATED_BODY()

	int32 Index = INDEX_NONE;
	int32 Amount = 0;
	bool bItemAtIndex = false;

public:

	FInventorySlotAvailability() {}
	FInventorySlotAvailability(int32 InIndex, int32 InAmount, bool bHasItem) : Index(InIndex), Amount(InAmount), bItemAtIndex(bHasItem) {}
};

USTRUCT()
struct FInventorySlotAvailabilityResult
{
	GENERATED_BODY()

	TWeakObjectPtr<class UInventoryItem> Item; // what the item
	int32 TotalRoomToFill = 0; // how much of the item we can fit
	int32 Remainder = 0; // how much we cannot fit
	bool bStackable = false; // whether it is stackable

	TArray<FInventorySlotAvailability> SlotAvailabilities;

public:

	FInventorySlotAvailabilityResult();

	static FInventorySlotAvailabilityResult Make(UInventoryItem* InItem, int32 InStartIndex);
	static FInventorySlotAvailabilityResult Make(UInventoryItem* InItem, int32 InStartIndex, int32 InMaxStackSize, int32 InAmount);
};

UENUM(BlueprintType)
enum class EInventoryTileQuadrant : uint8
{
	None,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
};

USTRUCT(BlueprintType)
struct FInventoryTileParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FIntPoint TileCoordinates {-1, -1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 TileIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	EInventoryTileQuadrant TileQuadrant = EInventoryTileQuadrant::None;

	FORCEINLINE_DEBUGGABLE
	bool operator==(const FInventoryTileParameters& Other) const
	{
		return TileCoordinates == Other.TileCoordinates && TileIndex == Other.TileIndex && TileQuadrant == Other.TileQuadrant;
	}
};

USTRUCT()
struct FInventorySpaceQueryResult
{
	GENERATED_BODY()

	// There is no items in the queried space
	bool bHasSpace = false;

	// Valid if there is a single item we can swap with
	TWeakObjectPtr<UInventoryItem> ValidItem = nullptr;

	// Upper left index of the valid item if there is one
	int32 UpperLeftIndex = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FInventoryStorageGridSlot
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TWeakObjectPtr<UInventoryItem> InventoryItem;

	UPROPERTY()
	int32 TileIndex = INDEX_NONE;
	UPROPERTY()
	int32 StackCount = 0;
	UPROPERTY()
	int32 StartIndex = INDEX_NONE; // upper left index where the actual stack count is stored

public:

	int32 GetTileIndex() const { return TileIndex; }
	void SetTileIndex(int32 InTileIndex) { TileIndex = InTileIndex; }

	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 InStackCount) { StackCount = InStackCount; }

	int32 GetStartIndex() const { return StartIndex; }
	void SetStartIndex(int32 InStartIndex) { StartIndex = InStartIndex; }

	const TWeakObjectPtr<UInventoryItem>& GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UInventoryItem* InItem);

	bool IsAvailable() const { return !InventoryItem.IsValid(); }

	void Clear()
	{
		StackCount = 0;
		StartIndex = INDEX_NONE;
		InventoryItem.Reset();
	}
};

UENUM()
enum class EInventoryEquipmentSlot : uint8
{
	Invalid = 255,
	Slot_0 = 0,
	Slot_1,
	Slot_2,
	Slot_3,
	Slot_4,
	Slot_5,
	Slot_6,
	Slot_7,
	Slot_8,
	Slot_9,
	Slot_10,
	Slot_11,
	Slot_12,
	Slot_13,
	Slot_14,
	Slot_15,
	MAX UMETA(Hidden),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EInventoryEquipmentSlot, EInventoryEquipmentSlot::Slot_0, EInventoryEquipmentSlot::Slot_15);

USTRUCT()
struct FInventoryEquipmentSlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TWeakObjectPtr<UInventoryItem> InventoryItem;

	// UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.EquipmentSlots"))
	// FGameplayTag EquipmentSlotTag;
	UPROPERTY(EditAnywhere, Category="Inventory")
	EInventoryEquipmentSlot SlotId = EInventoryEquipmentSlot::Invalid;

	// What type of equipment can be put into this slot
	// (e.g. "Equipment.Weapons" allows ant weapons, and "Equipment.Weapon.Sword" allows any sword but not axes) 
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems.Equipment"))
	FGameplayTag EquipmentTypeTag;

public:
	const TWeakObjectPtr<UInventoryItem>& GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UInventoryItem* InItem);

	bool IsAvailable() const { return !InventoryItem.IsValid(); }

	const FGameplayTag& GetEquipmentTypeTag() const { return EquipmentTypeTag; }
	//const FGameplayTag& GetEquipmentSlotTag() const { return EquipmentSlotTag; }
	EInventoryEquipmentSlot GetSlotId() const { return SlotId; }

	void Init(EInventoryEquipmentSlot NewSlotId, const FGameplayTag& NewEquipmentTypeTag)
	{
		SlotId = NewSlotId;
		EquipmentTypeTag = NewEquipmentTypeTag;
	}
	
	void Clear()
	{
		InventoryItem.Reset();
	}
};
