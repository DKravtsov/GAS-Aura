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

	virtual void SetupStorage(const FInventoryStorageSetupData* SetupData) {}

	virtual int32 GetItemIndex(UInventoryItem* Item) {return INDEX_NONE;};
	virtual void UpdateGridSlots(UInventoryItem* NewItem, int32 Index, bool bStackable, int32 StackAmount) {}
	virtual void RemoveItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex) {}

	//#if UE_WITH_CHEAT_MANAGER
	virtual void DebugPrintStorage() const {}
	//#endif//UE_WITH_CHEAT_MANAGER

protected:

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const;
};
