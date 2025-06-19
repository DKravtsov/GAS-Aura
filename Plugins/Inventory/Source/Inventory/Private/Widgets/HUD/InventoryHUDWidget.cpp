// Copyright 4sandwiches


#include "Widgets/HUD/InventoryHUDWidget.h"

void UInventoryHUDWidget::ShowPickupMessage(const FText& Message)
{
	BP_ShowPickupMessage(Message);
}

void UInventoryHUDWidget::HidePickupMessage()
{
	BP_HidePickupMessage();
}
