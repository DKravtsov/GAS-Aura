// Copyright 4sandwiches


#include "Widgets/Inventory/ViewModels//InventoryGridViewModel.h"

#include "DebugHelper.h"
#include "Inventory.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Widgets/Inventory/Spatial/InventoryGridWidget.h"

void UInventoryGridViewModel::Initialize(UInventoryGridWidget* InGridWidget, const FGameplayTag& ItemCategory)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Widget [%s] Category [%s]"), *InGridWidget->GetName(), *ItemCategory.ToString())
	
	APlayerController* PC = InGridWidget->GetOwningPlayer();
	check(PC);
	
	InventoryComponent = UInventoryStatics::GetInventoryComponent(PC);
	check(InventoryComponent.IsValid());
	
	const UInventorySpatialStorage* Storage = Cast<UInventorySpatialStorage>(InventoryComponent->GetInventoryStorage());
	checkf(Storage, TEXT("GridViewModel initialization failed. The InventoryStorage is null. InventoryComponent [%s]"),
		*GetNameSafe(InventoryComponent.Get()));

	StorageGrid = Storage->FindInventoryGridByCategory(ItemCategory);
	checkf(StorageGrid.IsValid(), TEXT("GridViewModel initialization failed. Couldn't find the inventory grid. InventoryComponent [%s]. Looked for [%s] but there are only: %s"),
				*GetNameSafe(InventoryComponent.Get()), *ItemCategory.ToString(), *Storage->GetInventoryGridNamesDebugString())

	GetOnItemAddedToGridDelegate().AddUObject(InGridWidget, &UInventoryGridWidget::HandleAddItemToGrid);
	GetOnStackChangedDelegate().AddUObject(InGridWidget, &UInventoryGridWidget::HandleOnStackChanged);
	GetOnGridSlotsResetDelegate().AddUObject(InGridWidget, &UInventoryGridWidget::HandleOnRemovedItemFromGrid);
	GetOnGridSlotsUpdatedDelegate().AddUObject(InGridWidget, &UInventoryGridWidget::HandleOnUpdateGridSlots);

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
