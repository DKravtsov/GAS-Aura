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

UInventoryEquippedSlottedItem* UInventoryEquippedGridSlot::OnItemEquipped(UInventoryItem* Item, const FGameplayTag& Tag, float TileSize)
{
	// Check the Equipment Type TagAdd commentMore actions
	// Get Grid Dimensions
	// Calculate the Draw Size for the Equipped Slotted Item
	// Create the Equipped Slotted Item widget
	// Set the Slotted Item's Inventory Item
	// Set the Slotted Item's Equipment Type Tag
	// Hide the Stack Count widget on the Slotted Item
	// Set Inventory Item on this class (the Equipped Grid Slot)
	// Set the Image Brush on the Equipped Slotted Item
	// Add the Slotted Item as a child to this widget's Overlay
	// Return the Equipped Slotted Item widget
}
