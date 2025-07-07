// Copyright 4sandwiches


#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"

#include "Inventory.h"
#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorageGrid.h"
#include "Items/Manifest/InventoryItemManifest.h"

void UInventorySpatialStorage::SetupStorage()
{
	InventoryGrids.Reserve(GridCategories.Num());
	
	for (const auto& ItemCategory : GridCategories)
	{
		auto NewGrid = NewObject<UInventorySpatialStorageGrid>(this, GetStorageGridClass());
		check(IsValid(NewGrid));
		NewGrid->ConstructGrid(Rows, Columns);
		
		InventoryGrids.Emplace(ItemCategory, NewGrid);
	}
}

TSubclassOf<UInventorySpatialStorageGrid> UInventorySpatialStorage::GetStorageGridClass()
{
	// TODO: should take this from dev settings
	return UInventorySpatialStorageGrid::StaticClass();
}

UInventorySpatialStorageGrid* UInventorySpatialStorage::FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const
{
	if (const auto* GridPtr = InventoryGrids.Find(ItemCategory))
	{
		return GridPtr->Get();
	}
	return nullptr;
}

FInventorySlotAvailabilityResult UInventorySpatialStorage::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	if (auto* Grid = FindInventoryGridByCategory(ItemManifest.GetItemCategory()))
		return Grid->HasRoomForItem(ItemManifest, StackCountOverride);
	
	UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid Item Category"));
	return FInventorySlotAvailabilityResult{};
}
