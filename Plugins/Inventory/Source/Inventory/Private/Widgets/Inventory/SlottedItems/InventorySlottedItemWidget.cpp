// Copyright 4sandwiches


#include "Widgets/Inventory/SlottedItems/InventorySlottedItemWidget.h"
#include "Items/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryManagement/Utils/InventoryStatics.h"

FReply UInventorySlottedItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, InMouseEvent);
	return FReply::Handled();
}

FReply UInventorySlottedItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInventorySlottedItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UInventoryStatics::ItemHovered(GetOwningPlayer(), InventoryItem.Get());
}

void UInventorySlottedItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UInventoryStatics::ItemUnhovered(GetOwningPlayer());
}

void UInventorySlottedItemWidget::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

void UInventorySlottedItemWidget::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UInventorySlottedItemWidget::UpdateStackCount(int32 NewStackCount)
{
	if (NewStackCount > 0)
	{
		Text_StackCount->SetText(FText::AsNumber(NewStackCount));
		if (!Text_StackCount->IsVisible())
		{
			Text_StackCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}
