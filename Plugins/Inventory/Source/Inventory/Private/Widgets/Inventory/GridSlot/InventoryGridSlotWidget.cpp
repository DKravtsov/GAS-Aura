// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryGridSlotWidget.h"
#include "Items/InventoryItem.h"
#include "Components/Image.h"

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
	
	OnGridSlotHovered.Broadcast(TileIndex, InMouseEvent);
}

void UInventoryGridSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnGridSlotUnhovered.Broadcast(TileIndex, InMouseEvent);
}

FReply UInventoryGridSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnGridSlotClicked.Broadcast(TileIndex, InMouseEvent);
	
	return FReply::Handled();
}

void UInventoryGridSlotWidget::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

