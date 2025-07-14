// Copyright 4sandwiches


#include "Widgets/Inventory/ViewModels//InventoryGridViewModel.h"

#include "DebugHelper.h"
#include "Inventory.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"

void UInventoryGridViewModel::Initialize(UInventoryComponent* InInventoryComponent, const FGameplayTag& ItemCategory)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Grid [%s]"), *ItemCategory.ToString())
	
	if (const auto Storage = Cast<UInventorySpatialStorage>(InInventoryComponent->GetInventoryStorage()))
	{
		if (const auto Grid = Storage->FindInventoryGridByCategory(ItemCategory))
		{
			Initialize(InInventoryComponent, Grid);
			return;
		}
		else
		{
			const FString AvailableGrids = Storage->GetInventoryGridNamesDebugString();
			UE_LOG(LogInventory, Error, TEXT("GridViewModel initialization failed. Couldn't find the inventory grid. InventoryComponent [%s]. Looked for [%s] but there are only: %s"),
				*GetNameSafe(InInventoryComponent), *ItemCategory.ToString(), *AvailableGrids);
			checkNoEntry();
		}
	}
	UE_LOG(LogInventory, Error, TEXT("GridViewModel initialization failed. The InventoryStorage is null. InventoryComponent [%s]"),
		*GetNameSafe(InInventoryComponent));
	checkNoEntry();
}

void UInventoryGridViewModel::Initialize(UInventoryComponent* InInventoryComponent, UInventoryStorageGrid* InGrid)
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

const FInventoryStorageGridSlot& UInventoryGridViewModel::GetGridSlot(int32 Index) const
{
	check(StorageGrid->IsValidIndex(Index));
	return StorageGrid->GetGridSlot(Index);
}

bool UInventoryGridViewModel::IsGridSlotAvailable(int32 Index) const
{
	return GetGridSlot(Index).IsAvailable();
}

void UInventoryGridViewModel::UpdateStackCount(int32 Index, int32 NewStackCount) const
{
	GetStorageGrid()->SetStackCount(Index, NewStackCount);
}

void UInventoryGridViewModel::UpdateGridSlots(UInventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount) const
{
	StorageGrid->UpdateGridSlots(NewItem, Index, bStackable, StackAmount);
}

void UInventoryGridViewModel::RemoveItemFromGrid(UInventoryItem* ItemToRemove, const int32 GridIndex) const
{
	StorageGrid->RemoveItemFromGrid(ItemToRemove, GridIndex);
}

void UInventoryGridViewModel::NotifyStackChanged(const FInventorySlotAvailabilityResult& Result) const
{
	StorageGrid->HandleStackChanged(Result);
}

FInventoryGridSlotsUpdatedSignature& UInventoryGridViewModel::GetOnGridSlotsUpdatedDelegate() const
{
	return StorageGrid->OnGridSlotsUpdated;
}

FInventoryGridSlotsUpdatedSignature& UInventoryGridViewModel::GetOnGridSlotsResetDelegate() const
{
	return StorageGrid->OnGridSlotsReset;
}
