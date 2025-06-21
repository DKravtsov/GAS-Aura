// Copyright 4sandwiches


#include "Widgets/Inventory/SlottedItems/InventorySlottedItemWidget.h"
#include "Items/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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
