// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryGridViewModel.h"

#include "Inventory.h"
#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorageGrid.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"

void UInventoryGridViewModel::Initialize(UInventoryComponent* InInventoryComponent, const FGameplayTag& ItemCategory)
{
	if (auto Storage = Cast<UInventorySpatialStorage>(InInventoryComponent->GetInventoryStorage()))
	{
		if (auto Grid = Storage->FindInventoryGridByCategory(ItemCategory))
		{
			Initialize(InInventoryComponent, Grid);
			return;
		}
	}
	UE_LOG(LogInventory, Error, TEXT("GridViewModel initialization failed. InventoryComponent [%s] grid [%s]"),
		*GetNameSafe(InInventoryComponent), *ItemCategory.ToString());
	checkNoEntry();
}

void UInventoryGridViewModel::Initialize(UInventoryComponent* InInventoryComponent, UInventorySpatialStorageGrid* InGrid)
{
	check(InventoryComponent == nullptr);
	
	InventoryComponent = InInventoryComponent;
	StorageGrid = InGrid;

	StorageGrid->OnItemAdded.AddLambda([this](const FInventorySlotAvailabilityResult& Result)
	{
		OnItemAdded.Broadcast(Result);
	});

	StorageGrid->OnStackChanged.AddLambda([this](const FInventorySlotAvailabilityResult& Result)
	{
		OnStackChanged.Broadcast(Result);
	});

	StorageGrid->OnItemRemoved.AddLambda([this](UInventoryItem* Item)
	{
		OnItemRemoved.Broadcast(Item);
	});
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

UInventoryComponent* UInventoryGridViewModel::GetInventoryComponent() const
{
	return InventoryComponent.Get();
}

const TArray<FInventoryStorageGridSlot>& UInventoryGridViewModel::GetGridSlots() const
{
	return StorageGrid->GetGridSlots();
}

const FInventoryStorageGridSlot& UInventoryGridViewModel::GetGridSlot(int32 Index) const
{
	check(StorageGrid->GetGridSlots().IsValidIndex(Index));
	return StorageGrid->GetGridSlots()[Index];
}

FInventoryStorageGridSlot& UInventoryGridViewModel::GetGridSlotMutable(int32 Index)
{
	check(StorageGrid->GetGridSlots().IsValidIndex(Index));
	return StorageGrid->GetGridSlotsMutable()[Index];
}
