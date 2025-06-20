// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryWidgetSpatial.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/Inventory/Spatial/InventoryGrid.h"

void UInventoryWidgetSpatial::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equipment->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowEquipmentGrid);
	Button_Consumable->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowConsumableGrid);
	Button_Crafting->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowCraftingGrid);

	ShowEquipmentGrid();
}

FInventorySlotAvailabilityResult UInventoryWidgetSpatial::HasRoomForItem(UInventoryItemComponent* ItemComponent) const
{
	return Super::HasRoomForItem(ItemComponent);
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
}
