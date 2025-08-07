// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryGridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "InventoryStorage.generated.h"

class UInventoryItemComponent;

USTRUCT(BlueprintType)
struct FInventoryStorageSetupData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<class UInventoryStorage> StorageClass;

public:

	// Explicitly declare all default stuff because I need virtual destructor
	
	FInventoryStorageSetupData() = default;
	FInventoryStorageSetupData(const FInventoryStorageSetupData&) = default;
	FInventoryStorageSetupData(FInventoryStorageSetupData&&) = default;
	FInventoryStorageSetupData& operator=(const FInventoryStorageSetupData&) = default;
	FInventoryStorageSetupData& operator=(FInventoryStorageSetupData&&) = default;
	virtual ~FInventoryStorageSetupData() {}

};

/**
 * 
 */
UCLASS(MinimalAPI, Abstract, CollapseCategories)
class UInventoryStorage : public UObject
{
	GENERATED_BODY()

public:

	bool HasRoomForItem(FInventorySlotAvailabilityResult& Result, const UInventoryItemComponent* ItemComponent) const;
	bool HasRoomForItem(FInventorySlotAvailabilityResult& Result, const struct FInventoryItemManifest& ItemManifest, const int32 StackCountOverride = -1) const;

	virtual bool HasRoomForItemAtIndex(FInventorySlotAvailabilityResult& Result, const struct FInventoryItemManifest& ItemManifest, const int32 Index, const int32 StackCountOverride = -1) const;

	virtual void SetupStorage(const FInventoryStorageSetupData* SetupData) {}

	virtual int32 GetItemIndex(UInventoryItem* Item) {return INDEX_NONE;}
	virtual int32 GetItemStackCount(UInventoryItem* Item, int32 GridIndex) {return 0;};
	virtual void SetItemStackCount(UInventoryItem* Item, int32 GridIndex, int32 NewStackCount) {}
	virtual void UpdateGridSlots(UInventoryItem* NewItem, int32 Index, bool bStackable, int32 StackAmount) {}
	virtual void RemoveItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex, int32 Count = -1) {}
	// returns remainder
	virtual int32 FillInStacksOrConsumeHover(UInventoryItem* Item, int32 TargetIndex, int32 AddStackCount) { return 0;}

	//#if UE_WITH_CHEAT_MANAGER
	virtual void DebugPrintStorage(FStringBuilderBase& Output, TFunctionRef<TCHAR (const UInventoryItem*)> ItemIndexFunc) const {}
	//#endif//UE_WITH_CHEAT_MANAGER

	virtual bool FindItemStacks(FInventorySlotAvailabilityResult& Result, UInventoryItem* Item, int32 TotalCount = 1) const { unimplemented(); return false;}

	virtual void MoveItem(UInventoryItem* Item, int32 SourceGridIndex, int32 TargetGridIndex) {}

protected:

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const;
};
