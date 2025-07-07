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

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Rows;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TArray<FGameplayTag> GridCategories;
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UInventorySpatialStorageGrid>> InventoryGrids;

public:

	virtual void SetupStorage() override;

protected:

	static TSubclassOf<UInventorySpatialStorageGrid> GetStorageGridClass();

	UInventorySpatialStorageGrid* FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const; 

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const override;
};
