// Copyright 4sandwiches


#include "Widgets/Inventory/HoverProxy/InventoryHoverItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/InventoryItem.h"

void UInventoryHoverItemWidget::SetImageBrush(const FSlateBrush& Brush)
{
	Image_Icon->SetBrush(Brush);
}

void UInventoryHoverItemWidget::UpdateStackCount(const int32 NewStackCount)
{
	StackCount = NewStackCount;
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

const FGameplayTag& UInventoryHoverItemWidget::GetItemType() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem->GetItemType();
	}
	return FGameplayTag::EmptyTag;
}

void UInventoryHoverItemWidget::SetIsStackable(bool bStackable)
{
	bIsStackable = bStackable;
	if (!bIsStackable)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryHoverItemWidget::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

void UInventoryHoverItemWidget::Reset()
{
	SetInventoryItem(nullptr);
	SetIsStackable(false);
	SetPreviousGridIndex(INDEX_NONE);
	UpdateStackCount(0);
	SetImageBrush(FSlateNoResource());
}
