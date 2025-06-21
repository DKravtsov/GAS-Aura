// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryGridSlot.h"

#include "Components/Image.h"

void UInventoryGridSlot::SetDefaultTexture()
{
	GridSlotState = EInventoryGridSlotState::Default;
	Image_GridSlot->SetBrush(DefaultBrush);
}

void UInventoryGridSlot::SetOccupiedTexture()
{
	GridSlotState = EInventoryGridSlotState::Occupied;
	Image_GridSlot->SetBrush(OccupiedBrush);
}

void UInventoryGridSlot::SetSelectedTexture()
{
	GridSlotState = EInventoryGridSlotState::Selected;
	Image_GridSlot->SetBrush(SelectedBrush);
}

void UInventoryGridSlot::SetGrayedOutTexture()
{
	GridSlotState = EInventoryGridSlotState::GrayedOut;
	Image_GridSlot->SetBrush(GrayedOutBrush);
}
