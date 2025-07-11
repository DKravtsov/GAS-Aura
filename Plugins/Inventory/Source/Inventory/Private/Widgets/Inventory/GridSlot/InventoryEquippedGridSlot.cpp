// Copyright 4sandwiches


#include "Widgets/Inventory/GridSlot/InventoryEquippedGridSlot.h"

#include "Inventory.h"
#include "InventoryGridTypes.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "EquipmentManagement/Components/InventoryEquipmentComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverItemWidget.h"
#include "Widgets/Inventory/SlottedItems/InventoryEquippedSlottedItemWidget.h"

bool UInventoryEquippedGridSlot::Bind(UInventoryEquipmentComponent* EquipComponent,	const FGameplayTag& EquipSlotTag)
{
	if (IsValid(EquipComponent) && EquipSlotTag.IsValid())
	{
		if (auto* FoundSlot = EquipComponent->FindEquipmentSlotByTag(EquipSlotTag))
		{
			EquipmentComponent = EquipComponent;
			EquipmentSlotTag = EquipSlotTag;
			return true;
		}
	}
	return false;
}

void UInventoryEquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnMouseEnter( InGeometry, InMouseEvent ); // call BP event directly because we don't call super

	if (!IsAvailable() || IsValid(EquippedSlottedItem))
		return;
	if (const auto HoverItem = UInventoryStatics::GetHoverItem(GetOwningPlayer()))
	{
		if (HoverItem->GetItemEquipmentTypeTag().MatchesTag(GetEquipmentTypeTag())) // partially match, e.g. "Item.Equipment.Sword" matches "Item.Equipment" 
		{
			SetOccupiedTexture();
			Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInventoryEquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnMouseLeave(InMouseEvent); // call BP event directly because we don't call super
	
	if (!IsAvailable() || IsValid(EquippedSlottedItem))
		return;
	if (const auto HoverItem = UInventoryStatics::GetHoverItem(GetOwningPlayer()))
	{
		if (HoverItem->GetItemEquipmentTypeTag().MatchesTag(GetEquipmentTypeTag())) // partially match, e.g. "Item.Equipment.Sword" matches "Item.Equipment" 
		{
			SetDefaultTexture();
			Image_GrayedOutIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

FReply UInventoryEquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, GetEquipmentTypeTag());
	return FReply::Handled();
}

const FGameplayTag& UInventoryEquippedGridSlot::GetEquipmentTypeTag() const
{
	if (const auto BoundSlot = GetBoundEquipmentSlot(); ensure(BoundSlot))
	{
		return BoundSlot->GetEquipmentTypeTag();
	}
	return FGameplayTag::EmptyTag;
}

void UInventoryEquippedGridSlot::SetGrayedIconBrush(const FSlateBrush& Brush)
{
	Image_GrayedOutIcon->SetBrush(Brush);
}

bool UInventoryEquippedGridSlot::IsAvailable() const
{
	const auto BoundSlot = GetBoundEquipmentSlot();
	return BoundSlot ? BoundSlot->IsAvailable() : false;
}

TWeakObjectPtr<UInventoryItem> UInventoryEquippedGridSlot::GetInventoryItem() const
{
	const auto BoundSlot = GetBoundEquipmentSlot();
	return BoundSlot ? BoundSlot->GetInventoryItem() : TWeakObjectPtr<UInventoryItem>();
}

void UInventoryEquippedGridSlot::SetInventoryItem(UInventoryItem* Item)
{
	//InventoryItem = Item;
}

UInventoryEquippedSlottedItemWidget* UInventoryEquippedGridSlot::OnItemEquipped(UInventoryItem* Item, const FGameplayTag& Tag, float TileSize)
{
	// Check the Equipment Type TagAdd commentMore actions
	if (!Tag.MatchesTagExact(GetEquipmentTypeTag()))
		return nullptr;
	
	// Get Grid Dimensions
	const auto GridFragment = UInventoryItem::GetFragment<FInventoryItemGridFragment>(Item, InventoryFragmentTags::FragmentTag_Grid);
	if (!GridFragment)
		return nullptr;

	// Create the Equipped Slotted Item widget
	EquippedSlottedItem = CreateWidget<UInventoryEquippedSlottedItemWidget>(GetOwningPlayer(), EquippedSlottedItemClass);
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
		OverlaySlot->SetHorizontalAlignment(HAlign_Center);
		OverlaySlot->SetVerticalAlignment(VAlign_Center);
		SetOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	return EquippedSlottedItem;
}

void UInventoryEquippedGridSlot::UpdateIfPending()
{
	if (bPendingEquipping)
	{
		PendingEquippingFunction();
		PendingEquippingFunction.Reset();
		bPendingEquipping = false;
	}
}

const FInventoryEquipmentSlot* UInventoryEquippedGridSlot::GetBoundEquipmentSlot() const
{
	return EquipmentComponent.IsValid() ? EquipmentComponent->FindEquipmentSlotByTag(EquipmentSlotTag) : nullptr;
}

void UInventoryEquippedGridSlot::ClearEquippedSlot()
{
	//SetInventoryItem(nullptr);
	EquippedSlottedItem = nullptr;
	PendingEquippingFunction.Reset();
	bPendingEquipping = false;
}
