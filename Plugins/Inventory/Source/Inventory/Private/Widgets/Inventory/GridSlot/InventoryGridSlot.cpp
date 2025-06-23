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

void UInventoryGridSlot::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

