// Copyright 4sandwiches


#include "Widgets/Inventory/SlottedItems/InventorySlottedItemWidget.h"
#include "Items/InventoryItem.h"
#include "Components/Image.h"

void UInventorySlottedItemWidget::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

void UInventorySlottedItemWidget::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}
