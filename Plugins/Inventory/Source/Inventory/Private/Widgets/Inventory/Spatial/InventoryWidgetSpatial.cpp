// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryWidgetSpatial.h"

#include "Inventory.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Widgets/Inventory/Spatial/InventoryGrid.h"
#include "Widgets/ItemDescription/InventoryItemDescription.h"

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

	FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, ItemDescWidget]()
	{
		//ItemDescWidget->Show();
		SetToolTip(ItemDescWidget);
	});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(TimerHandle_Description, TimerDelegate, DescriptionDelay, false);
}

void UInventoryWidgetSpatial::OnInventoryUnhovered()
{
	//GetOrCreateItemDescription()->Hide();
	SetToolTip(nullptr);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(TimerHandle_Description);	
}

bool UInventoryWidgetSpatial::HasHoverItem() const
{
	return ActiveGrid->HasHoverItem();
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
	if (ItemCategory.MatchesTag(InventoryTags::Inventory_ItemCategory_Equipment))
		return InventoryGrid_Equipment;
	if (ItemCategory.MatchesTag(InventoryTags::Inventory_ItemCategory_Consumable))
		return InventoryGrid_Consumable;
	if (ItemCategory.MatchesTag(InventoryTags::Inventory_ItemCategory_Crafting))
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
