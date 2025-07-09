// Copyright 4sandwiches


#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"

#include "Inventory.h"
#include "InventoryGlobalSettings.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"
#include "Items/Manifest/InventoryItemManifest.h"

void UInventorySpatialStorage::SetupStorage()
{
	InventoryGrids.Reserve(GridCategories.Num());

	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOuter());
	check(InventoryComponent);
	AActor* OwningActor = InventoryComponent->GetOwner();
	check(OwningActor && OwningActor->HasAuthority());
	
	LOG_NETFUNCTIONCALL_OWNER(OwningActor)
	
	UClass* GridClass = GetStorageGridClass();
	check(GridClass != nullptr);
	
	for (const auto& ItemCategory : GridCategories)
	{
		auto NewGrid = NewObject<UInventoryStorageGrid>(OwningActor, GridClass);
		check(IsValid(NewGrid));
		NewGrid->SetItemCategory(ItemCategory);
		NewGrid->ConstructGrid(Rows, Columns);
		
		InventoryGrids.Emplace(ItemCategory, NewGrid);

		if (InventoryComponent)
		{
			InventoryComponent->AddRepSubObj(NewGrid);
		}
	}
}

TSubclassOf<UInventoryStorageGrid> UInventorySpatialStorage::GetStorageGridClass()
{
	return UInventoryGlobalSettings::GetSpatialStorageGridClass();
}

UInventoryStorageGrid* UInventorySpatialStorage::FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const
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
