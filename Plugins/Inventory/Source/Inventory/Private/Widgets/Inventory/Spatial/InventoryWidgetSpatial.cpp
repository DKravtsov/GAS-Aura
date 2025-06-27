// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryWidgetSpatial.h"

#include "Inventory.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Widgets/Inventory/GridSlot/InventoryEquippedGridSlot.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverProxy.h"
#include "Widgets/Inventory/SlottedItems/InventoryEquippedSlottedItem.h"
#include "Widgets/Inventory/Spatial/InventoryGrid.h"
#include "Widgets/ItemDescription/InventoryItemDescription.h"
#include "Inventory.h"

void UInventoryWidgetSpatial::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equipment->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowEquipmentGrid);
	Button_Consumable->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowConsumableGrid);
	Button_Crafting->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowCraftingGrid);

	InventoryGrid_Equipment->SetOwningCanvas(CanvasPanel);
	InventoryGrid_Consumable->SetOwningCanvas(CanvasPanel);
	InventoryGrid_Crafting->SetOwningCanvas(CanvasPanel);
	
	ShowEquipmentGrid();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		if (auto GridSlot = Cast<UInventoryEquippedGridSlot>(Widget))
		{
			EquippedGridSlots.Emplace(GridSlot);
			GridSlot->EquippedGridSlotClicked.AddDynamic(this, &UInventoryWidgetSpatial::EquippedGridSlotClicked);
		}
	});
}

FReply UInventoryWidgetSpatial::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->DropHoverItemOnGround();
	}
	return FReply::Handled();
}

FInventorySlotAvailabilityResult UInventoryWidgetSpatial::HasRoomForItem(UInventoryItemComponent* ItemComponent) const
{
	 if (const UInventoryGrid* Grid = GetInventoryGridByCategory(UInventoryStatics::GetItemCategory(ItemComponent)))
	 {
		 return Grid->HasRoomForItem(ItemComponent);
	 }
	UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid Item Category"));
	return FInventorySlotAvailabilityResult();
}

void UInventoryWidgetSpatial::OnInventoryHovered(UInventoryItem* Item)
{
	auto ItemDescWidget = GetOrCreateItemDescription();
	//ItemDescWidget->Hide();
	SetToolTip(nullptr);

	const auto& Manifest = Item->GetItemManifest();
	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, &Manifest, ItemDescWidget]()
	{
		//ItemDescWidget->Show();
		SetToolTip(ItemDescWidget);

		Manifest.AssimilateInventoryFragments(ItemDescWidget);
	});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(TimerHandle_Description, TimerDelegate, DescriptionDelay, false);
}

void UInventoryWidgetSpatial::OnInventoryUnhovered()
{
	//GetOrCreateItemDescription()->Hide();
	GetOrCreateItemDescription()->Collapse();
	SetToolTip(nullptr);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(TimerHandle_Description);	
}

bool UInventoryWidgetSpatial::HasHoverItem() const
{
	return ActiveGrid->HasHoverItem();
}

UInventoryHoverProxy* UInventoryWidgetSpatial::GetHoverItem() const
{
	return ActiveGrid.IsValid() ? ActiveGrid->GetHoverItem() : nullptr;
}

float UInventoryWidgetSpatial::GetTileSize() const
{
	return ActiveGrid.IsValid() ? ActiveGrid->GetTileSize() : 0.f;
}

void UInventoryWidgetSpatial::ShowEquipmentGrid()
{
	SetActiveGrid(InventoryGrid_Equipment, Button_Equipment);
}

void UInventoryWidgetSpatial::ShowConsumableGrid()
{
	SetActiveGrid(InventoryGrid_Consumable, Button_Consumable);
}

void UInventoryWidgetSpatial::ShowCraftingGrid()
{
	SetActiveGrid(InventoryGrid_Crafting, Button_Crafting);
}

void UInventoryWidgetSpatial::EquippedGridSlotClicked(UInventoryEquippedGridSlot* GridSlot, const FGameplayTag& EquipmentTypeTag)
{
	// Check to see if we can equip the Hover ItemAdd commentMore actions
	if (!CanEquipHoverItem(GridSlot, EquipmentTypeTag))
		return;

	// Create an Equipped Slotted Item and add it to the Equipped Grid Slot
	const float TileSize = GetTileSize();
	UInventoryItem* ItemToEquip = GetHoverItem()->GetInventoryItem();
	if (UInventoryEquippedSlottedItem* EquippedSlottedItem = GridSlot->OnItemEquipped(ItemToEquip, EquipmentTypeTag, TileSize))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &UInventoryWidgetSpatial::EquippedSlottedItemClicked);

		InventoryGrid_Equipment->ClearHoverItem();

		// Inform the server that we've equipped an item (potentially unequipping an item as well)
		UInventoryComponent* InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
		check(IsValid(InventoryComponent));

		InventoryComponent->EquipItem(ItemToEquip, nullptr);
	}
}

void UInventoryWidgetSpatial::EquippedSlottedItemClicked(UInventoryEquippedSlottedItem* SlottedItem)
{
	
}

void UInventoryWidgetSpatial::DisableButton(UButton* Button)
{
	Button_Equipment->SetIsEnabled(true);
	Button_Consumable->SetIsEnabled(true);
	Button_Crafting->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UInventoryWidgetSpatial::SetActiveGrid(UInventoryGrid* Grid, UButton* Button)
{
	DisableButton(Button);
	GridSwitcher->SetActiveWidget(Grid);
	ActiveGrid = Grid;
}

UInventoryGrid* UInventoryWidgetSpatial::GetInventoryGridByCategory(const FGameplayTag& ItemCategory) const
{
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment))
		return InventoryGrid_Equipment;
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Consumable))
		return InventoryGrid_Consumable;
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Crafting))
		return InventoryGrid_Crafting;
	return nullptr;
}

UInventoryItemDescription* UInventoryWidgetSpatial::GetOrCreateItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInventoryItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		//CanvasPanel->AddChildToCanvas(ItemDescription);
	}
	return ItemDescription;
}

bool UInventoryWidgetSpatial::CanEquipHoverItem(UInventoryEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag)
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid())
		return false;

	UInventoryHoverProxy* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem))
		return false;

	UInventoryItem* HeldItem = HoverItem->GetInventoryItem();
	return IsValid(HeldItem)
		&& !HoverItem->IsStackable() && HeldItem->IsEquipable()
		&& HeldItem->GetItemType().MatchesTag(EquipmentTypeTag);
}
