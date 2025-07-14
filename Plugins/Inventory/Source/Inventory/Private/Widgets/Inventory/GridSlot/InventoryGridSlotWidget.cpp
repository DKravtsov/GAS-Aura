// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryGridSlotWidget.h"
#include "Items/InventoryItem.h"
#include "Components/Image.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"

void UInventoryGridSlotWidgetBase::SetDefaultTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Default;
	Image_GridSlot->SetBrush(DefaultBrush);
}

void UInventoryGridSlotWidgetBase::SetOccupiedTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Occupied;
	Image_GridSlot->SetBrush(OccupiedBrush);
}

void UInventoryGridSlotWidgetBase::SetSelectedTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Selected;
	Image_GridSlot->SetBrush(SelectedBrush);
}

void UInventoryGridSlotWidgetBase::SetGrayedOutTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::GrayedOut;
	Image_GridSlot->SetBrush(GrayedOutBrush);
}

void UInventoryGridSlotWidgetBase::SetGridSlotState(EInventoryGridSlotVisualState NewState)
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

void UInventoryGridSlotWidgetBase::NativePreConstruct()
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
	check(InStorageGrid->IsValidIndex(InTileIndex));
	StorageGrid = InStorageGrid;
	GridIndex = InTileIndex;
}

const FInventoryStorageGridSlot& UInventoryGridSlotWidget::GetStorageSlot() const
{
	return StorageGrid->GetGridSlot(GridIndex);
}

int32 UInventoryGridSlotWidget::GetStackCount() const
{
	return GetStorageSlot().GetStackCount();
}

int32 UInventoryGridSlotWidget::GetStartIndex() const
{
	return GetStorageSlot().GetStartIndex();
}

TWeakObjectPtr<UInventoryItem> UInventoryGridSlotWidget::GetInventoryItem() const
{
	return GetStorageSlot().GetInventoryItem();
}

bool UInventoryGridSlotWidget::IsAvailable() const
{
	return GridIndex != INDEX_NONE ? GetStorageSlot().IsAvailable() : bIsAvailable;
}

