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

void UInventoryGridSlot::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

