// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryGridSlot.h"
#include "Items/InventoryItem.h"
#include "Components/Image.h"

void UInventoryGridSlot::SetDefaultTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Default;
	Image_GridSlot->SetBrush(DefaultBrush);
}

void UInventoryGridSlot::SetOccupiedTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Occupied;
	Image_GridSlot->SetBrush(OccupiedBrush);
}

void UInventoryGridSlot::SetSelectedTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::Selected;
	Image_GridSlot->SetBrush(SelectedBrush);
}

void UInventoryGridSlot::SetGrayedOutTexture()
{
	GridSlotState = EInventoryGridSlotVisualState::GrayedOut;
	Image_GridSlot->SetBrush(GrayedOutBrush);
}

void UInventoryGridSlot::SetGridSlotState(EInventoryGridSlotVisualState NewState)
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

void UInventoryGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	OnGridSlotHovered.Broadcast(TileIndex, InMouseEvent);
}

void UInventoryGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnGridSlotUnhovered.Broadcast(TileIndex, InMouseEvent);
}

FReply UInventoryGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnGridSlotClicked.Broadcast(TileIndex, InMouseEvent);
	
	return FReply::Handled();
}

void UInventoryGridSlot::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

