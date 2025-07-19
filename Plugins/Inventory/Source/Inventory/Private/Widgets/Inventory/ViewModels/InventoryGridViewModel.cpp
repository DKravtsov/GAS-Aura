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
	LOG_NETFUNCTIONCALL_MSG(TEXT("Index: %d, NewStackCount: %d"), Index, NewStackCount)

	if (!HasAuthority())
	{
		GetStorageGrid()->SetStackCount(Index, NewStackCount);
	}
}

void UInventoryGridViewModel::UpdateGridSlots(UInventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount) const
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item: [%s], GridIndex: %d"), *GetInventoryItemId(NewItem), Index)
	
	InventoryComponent->Server_UpdateSlots(NewItem, Index, bStackable, StackAmount);
}

void UInventoryGridViewModel::RemoveItemFromGrid(UInventoryItem* ItemToRemove, const int32 GridIndex) const
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item: [%s], GridIndex: %d"), *GetInventoryItemId(ItemToRemove), GridIndex)
	
	StorageGrid->RemoveItemFromGrid(ItemToRemove, GridIndex);
	if (!HasAuthority())
	{
		InventoryComponent->Server_RemoveFromStorage(ItemToRemove, GridIndex);
	}
}

void UInventoryGridViewModel::AssignHoverItem(UInventoryItem* InventoryItem, int32 GridIndex, int32 PrevGridIndex)
{
	LOG_NETFUNCTIONCALL

	if (!HasAuthority())
	{
		InventoryComponent->Server_AssignHoverItem(InventoryItem, GridIndex, PrevGridIndex);
	}
}

void UInventoryGridViewModel::ClearHoverItem()
{
	LOG_NETFUNCTIONCALL
	
	if (!HasAuthority())
	{
		InventoryComponent->Server_ClearHoverItem();
	}
}

void UInventoryGridViewModel::PutDownHoverItem(const FInventorySlotAvailabilityResult& Result)
{
	LOG_NETFUNCTIONCALL

	if (HasAuthority())
	{
		StorageGrid->HandleStackChanged(Result);
	}
	else
	{// ignore the Result, the server knows better
		InventoryComponent->Server_PutDownHoverItem();
	}
}

void UInventoryGridViewModel::FillInStacksOrConsumeHover(UInventoryItem* Item, int32 TargetIndex, int32 SourceIndex)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s] from %d to %d"), *GetInventoryItemId(Item), SourceIndex, TargetIndex)
	if (!HasAuthority())
	{
		InventoryComponent->Server_FillInStacksOrConsumeHover(Item, TargetIndex, SourceIndex);
	}
	
}

void UInventoryGridViewModel::SplitStackToHoverItem(UInventoryItem* InventoryItem, int32 GridIndex, int32 SplitAmount)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item[%s] index: %d, SplitAmount = %d"), *GetInventoryItemId(InventoryItem), GridIndex, SplitAmount)
	if (!HasAuthority())
	{
		InventoryComponent->Server_SplitStackToHoverItem(InventoryItem, GridIndex, SplitAmount);
	}
}

void UInventoryGridViewModel::SwapStackCountWithHoverItem(UInventoryItem* InventoryItem, int32 GridIndex)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item[%s] index: %d"), *GetInventoryItemId(InventoryItem), GridIndex)
	if (!HasAuthority())
	{
		InventoryComponent->Server_SwapStackCountWithHoverItem(InventoryItem, GridIndex);
	}
}

bool UInventoryGridViewModel::HasAuthority() const
{
	return InventoryComponent.IsValid() && InventoryComponent->GetOwner()->HasAuthority();
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
