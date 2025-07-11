// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "InventorySpatialStorage.generated.h"

class UInventoryStorageGrid;

/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable)
class UInventorySpatialStorage : public UInventoryStorage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(UIMin=1, ClampMin=1))
	int32 Rows = 1;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(UIMin=1, ClampMin=1))
	int32 Columns = 1;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.ItemCategory"))
	TArray<FGameplayTag> GridCategories;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UInventoryStorageGrid>> InventoryGrids;

public:

	virtual void SetupStorage() override;

	UInventoryStorageGrid* FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const; 

	//~ Begin of UInventoryStorage interface
	virtual int32 GetItemIndex(UInventoryItem* Item) override;
	virtual void UpdateGridSlots(UInventoryItem* NewItem, int32 Index, bool bStackable, int32 StackAmount) override;
	virtual void RemoveItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex) override;
	//~ End of UInventoryStorage interface

protected:

	static TSubclassOf<UInventoryStorageGrid> GetStorageGridClass();

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const override;
};
