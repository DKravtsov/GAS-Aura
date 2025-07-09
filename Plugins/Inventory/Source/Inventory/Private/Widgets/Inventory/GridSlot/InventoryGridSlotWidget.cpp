// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryGridSlotWidget.h"
#include "Items/InventoryItem.h"
#include "Components/Image.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"

void UInventoryGridSlotWidget::SetDefaultTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Default;
	Image_GridSlot->SetBrush(DefaultBrush);
}

void UInventoryGridSlotWidget::SetOccupiedTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Occupied;
	Image_GridSlot->SetBrush(OccupiedBrush);
}

void UInventoryGridSlotWidget::SetSelectedTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Selected;
	Image_GridSlot->SetBrush(SelectedBrush);
}

void UInventoryGridSlotWidget::SetGrayedOutTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::GrayedOut;
	Image_GridSlot->SetBrush(GrayedOutBrush);
}

void UInventoryGridSlotWidget::SetGridSlotState(EInventoryGridSlotVisualState NewState)
{
	switch (NewState)
	{
	case EInventoryGridSlotVisualState::Default: SetDefaultTexture(); break;
	case EInventoryGridSlotVisualState::Occupied: SetOccupiedTexture(); break;
	case EInventoryGridSlotVisualState::Selected: SetSelectedTexture(); break;
	case EInventoryGridSlotVisualState::GrayedOut: SetGrayedOutTexture(); break;
	default:
		checkNoEntry();
	}
}

void UInventoryGridSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image_GridSlot->SetBrush(DefaultBrush);
}

void UInventoryGridSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	OnGridSlotHovered.Broadcast(GetTileIndex(), InMouseEvent);
}

void UInventoryGridSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnGridSlotUnhovered.Broadcast(GetTileIndex(), InMouseEvent);
}

FReply UInventoryGridSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnGridSlotClicked.Broadcast(GetTileIndex(), InMouseEvent);
	
	return FReply::Handled();
}

void UInventoryGridSlotWidget::Bind(UInventoryStorageGrid* InStorageGrid, int32 InTileIndex)
{
	check(IsValid(InStorageGrid));
	StorageGrid = InStorageGrid;
	GridIndex = InTileIndex;
	check(StorageGrid->GetGridSlots().IsValidIndex(GridIndex));
}

const FInventoryStorageGridSlot& UInventoryGridSlotWidget::GetStorageSlot() const
{
	return StorageGrid->GetGridSlot(GridIndex);
}

FInventoryStorageGridSlot& UInventoryGridSlotWidget::GetStorageSlotMutable()
{
	return StorageGrid->GetGridSlotMutable(GridIndex);
}

int32 UInventoryGridSlotWidget::GetStackCount() const
{
	//return StackCount;
	return GetStorageSlot().GetStackCount();
}

int32 UInventoryGridSlotWidget::GetStartIndex() const
{
	//return StartIndex;
	return GetStorageSlot().GetStartIndex();
}

TWeakObjectPtr<UInventoryItem> UInventoryGridSlotWidget::GetInventoryItem() const
{
	//return InventoryItem;
	return GetStorageSlot().GetInventoryItem();
}

bool UInventoryGridSlotWidget::IsAvailable() const
{
	return GridIndex != INDEX_NONE ? GetStorageSlot().IsAvailable() : bIsAvailable;
}

// void UInventoryGridSlotWidget::SetInventoryItem(UInventoryItem* Item)
// {
// 	InventoryItem = Item;
// }

