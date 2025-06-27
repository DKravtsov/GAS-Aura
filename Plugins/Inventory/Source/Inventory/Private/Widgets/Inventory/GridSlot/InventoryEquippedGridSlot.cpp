// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryEquippedGridSlot.h"

#include "Components/Image.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverProxy.h"

void UInventoryEquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnMouseEnter( InGeometry, InMouseEvent ); // call BP event directly because we don't call super

	if (!IsAvailable())
		return;
	if (const auto HoverItem = UInventoryStatics::GetHoverItem(GetOwningPlayer()))
	{
		if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag)) // partially match, e.g. "Item.Equipment.Sword" matches "Item.Equipment" 
		{
			SetOccupiedTexture();
			Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInventoryEquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnMouseLeave(InMouseEvent); // call BP event directly because we don't call super
	
	if (!IsAvailable())
		return;
	if (const auto HoverItem = UInventoryStatics::GetHoverItem(GetOwningPlayer()))
	{
		if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag)) // partially match, e.g. "Item.Equipment.Sword" matches "Item.Equipment" 
		{
			SetDefaultTexture();
			Image_GrayedOutIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

FReply UInventoryEquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return FReply::Handled();
}

void UInventoryEquippedGridSlot::SetGrayedIconBrush(const FSlateBrush& Brush)
{
	Image_GrayedOutIcon->SetBrush(Brush);
}
