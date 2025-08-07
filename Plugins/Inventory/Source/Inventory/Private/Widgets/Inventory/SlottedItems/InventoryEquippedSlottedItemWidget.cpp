// Copyright 4sandwiches


#include "Widgets/Inventory/SlottedItems/InventoryEquippedSlottedItemWidget.h"

#include "DebugHelper.h"

FReply UInventoryEquippedSlottedItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
		return FReply::Unhandled();
	
	BROADCAST_WITH_LOG(OnEquippedSlottedItemClicked, this);
	return FReply::Handled();
}

FReply UInventoryEquippedSlottedItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
