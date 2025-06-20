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
	
}

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
};