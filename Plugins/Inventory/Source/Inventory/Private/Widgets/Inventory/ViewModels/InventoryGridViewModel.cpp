// Copyright 4sandwiches


#include "Widgets/Inventory/ViewModels//InventoryGridViewModel.h"

#include "DebugHelper.h"
#include "Inventory.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"
#include "InventoryManagement/Utils/InventoryStatics.h"

void UInventoryGridViewModel::Initialize(const APlayerController* OwningPlayer, UInventoryStorage* Storage, const FGameplayTag& ItemCategory)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Category [%s]"), *ItemCategory.ToString())
	
	auto InventoryComponent = UInventoryStatics::GetInventoryComponent(OwningPlayer);
	check(IsValid(InventoryComponent));
	
	const UInventorySpatialStorage* SpatialStorage = Cast<UInventorySpatialStorage>(Storage);
	checkf(SpatialStorage, TEXT("GridViewModel initialization failed. The InventoryStorage is null. InventoryComponent [%s]"),
		*GetNameSafe(InventoryComponent));

	StorageGrid = SpatialStorage->FindInventoryGridByCategory(ItemCategory);
	checkf(StorageGrid.IsValid(), TEXT("GridViewModel initialization failed. Couldn't find the inventory grid. InventoryComponent [%s]. Looked for [%s] but there are only: %s"),
				*GetNameSafe(InventoryComponent), *ItemCategory.ToString(), *SpatialStorage->GetInventoryGridNamesDebugString())
	
}

int32 UInventoryGridViewModel::GetRows() const
{
	return StorageGrid->GetRows();
}

int32 UInventoryGridViewModel::GetColumns() const
{
	return StorageGrid->GetColumns();
}

const FGameplayTag& UInventoryGridViewModel::GetInventoryCategory() const
{
	return StorageGrid->GetItemCategory();
}

const FInventoryStorageGridSlot& UInventoryGridViewModel::GetGridSlot(int32 Index) const
{
	check(StorageGrid->IsValidIndex(Index));
	return StorageGrid->GetGridSlot(Index);
}

bool UInventoryGridViewModel::IsGridSlotAvailable(int32 Index) const
{
	return GetGridSlot(Index).IsAvailable();
}

FInventoryItemGridChangedDelegate& UInventoryGridViewModel::GetOnItemAddedToGridDelegate() const
{
	return StorageGrid->OnItemAddedToGrid;
}

FInventoryItemChangedDelegate& UInventoryGridViewModel::GetOnItemRemovedDelegate() const
{
	return StorageGrid->OnItemRemoved;
}

FInventoryItemGridChangedDelegate& UInventoryGridViewModel::GetOnStackChangedDelegate() const
{
	return StorageGrid->OnStackChanged;
}

FInventoryGridSlotsUpdatedSignature& UInventoryGridViewModel::GetOnGridSlotsUpdatedDelegate() const
{
	return StorageGrid->OnGridSlotsUpdated;
}

FInventoryGridSlotsUpdatedSignature& UInventoryGridViewModel::GetOnGridSlotsResetDelegate() const
{
	return StorageGrid->OnGridSlotsReset;
}
