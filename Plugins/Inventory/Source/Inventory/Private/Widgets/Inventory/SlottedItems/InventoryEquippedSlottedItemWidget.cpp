// Copyright 4sandwiches


#include "Widgets/Inventory/SlottedItems/InventoryEquippedSlottedItemWidget.h"

FReply UInventoryEquippedSlottedItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEquippedSlottedItemClicked.Broadcast(this);
	return FReply::Handled();
}

FReply UInventoryEquippedSlottedItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
