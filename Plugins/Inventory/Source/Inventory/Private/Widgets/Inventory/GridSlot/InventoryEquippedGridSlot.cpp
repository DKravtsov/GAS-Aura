// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryEquippedGridSlot.h"

#include "Inventory.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverProxy.h"
#include "Widgets/Inventory/SlottedItems/InventoryEquippedSlottedItem.h"

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
	if (!Tag.MatchesTagExact(EquipmentTypeTag))
		return nullptr;
	
	// Get Grid Dimensions
	const auto GridFragment = UInventoryItem::GetFragment<FInventoryItemGridFragment>(Item, InventoryFragmentTags::FragmentTag_Grid);
	if (!GridFragment)
		return nullptr;

	// Create the Equipped Slotted Item widget
	EquippedSlottedItem = CreateWidget<UInventoryEquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);
	check(EquippedSlottedItem);
	EquippedSlottedItem->SetInventoryItem(Item);
	EquippedSlottedItem->SetEquipmentTypeTag(Tag);
	EquippedSlottedItem->UpdateStackCount(0); // Hide the Stack Count widget on the Slotted Item

	// Set Inventory Item on this class (the Equipped Grid Slot)
	SetInventoryItem(Item);

	// Calculate the Draw Size for the Equipped Slotted Item
	const FVector2D DrawSize = UInventoryWidgetUtils::GetItemDrawSize(Item, TileSize);
	
	// Set the Image Brush on the Equipped Slotted Item
	if (auto ImageFragment = UInventoryWidgetUtils::GetImageFragmentFromInventoryItem(Item))
	{
		ImageFragment->GetIcon().LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateLambda([DrawSize, this](const FSoftObjectPath& SoftPath, UObject* LoadedObject)
		{
			if (!IsValid(LoadedObject))
			{
				UE_LOG(LogInventory, Error, TEXT("Loading failed: [%s]"), *SoftPath.ToString());
				return;
			}
			FSlateBrush ImageBrush;
			ImageBrush.SetResourceObject(LoadedObject);
			ImageBrush.DrawAs = ESlateBrushDrawType::Image;
			ImageBrush.ImageSize = DrawSize;
			EquippedSlottedItem->SetImageBrush(ImageBrush);
		}));

		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Brush.ImageSize = FVector2D{1,1};
		EquippedSlottedItem->SetImageBrush(Brush);
	}
	
	// Add the Slotted Item as a child to this widget's Overlay
	if (auto OverlaySlot = Overlay_Root->AddChildToOverlay(EquippedSlottedItem))
	{
		FGeometry OverlayGeometry = Overlay_Root->GetCachedGeometry();
		auto OverlayPos = OverlayGeometry.Position;
		auto OverlaySize = OverlayGeometry.Size;

		const float LeftPadding = OverlaySize.X / 2.f - DrawSize.X / 2.f;
		const float TopPadding = OverlaySize.Y / 2.f - DrawSize.Y / 2.f;

		OverlaySlot->SetPadding(FMargin{LeftPadding, TopPadding});
	}
	
	return EquippedSlottedItem;
}
