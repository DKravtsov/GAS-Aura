﻿// Copyright 4sandwiches

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
	FIntPoint TileCoordinates;

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
