// Copyright 4sandwiches


#include "Widgets/Inventory/SlottedItems/InventoryEquippedSlottedItem.h"

FReply UInventoryEquippedSlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEquippedSlottedItemClicked.Broadcast(this);
	return FReply::Handled();
}

FReply UInventoryEquippedSlottedItem::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
