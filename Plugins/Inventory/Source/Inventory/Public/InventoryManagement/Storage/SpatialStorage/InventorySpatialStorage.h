// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "InventorySpatialStorage.generated.h"

class UInventorySpatialStorageGrid;

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
	TMap<FGameplayTag, TObjectPtr<UInventorySpatialStorageGrid>> InventoryGrids;

public:

	virtual void SetupStorage() override;

protected:

	static TSubclassOf<UInventorySpatialStorageGrid> GetStorageGridClass();

	UInventorySpatialStorageGrid* FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const; 

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const override;
};
