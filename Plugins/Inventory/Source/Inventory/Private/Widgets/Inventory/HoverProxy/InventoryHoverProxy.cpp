// Copyright 4sandwiches


#include "Widgets/Inventory/HoverProxy/InventoryHoverProxy.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/InventoryItem.h"

void UInventoryHoverProxy::SetImageBrush(const FSlateBrush& Brush)
{
	Image_Icon->SetBrush(Brush);
}

void UInventoryHoverProxy::UpdateStackCount(const int32 NewStackCount)
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

const FGameplayTag& UInventoryHoverProxy::GetItemType() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem->GetItemType();
	}
	return FGameplayTag::EmptyTag;
}

void UInventoryHoverProxy::SetIsStackable(bool bStackable)
{
	bIsStackable = bStackable;
	if (!bIsStackable)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryHoverProxy::SetInventoryItem(UInventoryItem* Item)
{
	InventoryItem = Item;
}

void UInventoryHoverProxy::Reset()
{
	SetInventoryItem(nullptr);
	SetIsStackable(false);
	SetPreviousGridIndex(INDEX_NONE);
	UpdateStackCount(0);
	SetImageBrush(FSlateNoResource());
}
