// Copyright 4sandwiches


#include "Widgets/Store/InventoryStoreWidgetSpatial.h"

#include "DebugHelper.h"
#include "Components/WidgetSwitcher.h"
#include "Store/Components/InventoryStoreComponent.h"
#include "Widgets/Inventory/Spatial/InventoryGridWidget.h"

bool UInventoryStoreWidgetSpatial::Initialize()
{
	return Super::Initialize();
}

void UInventoryStoreWidgetSpatial::NativeOnInitialized()
{
	LOG_NETFUNCTIONCALL
	
	GetOnGridWidgetSwitchedDelegate().AddUObject(this, &UInventoryStoreWidgetSpatial::HandleOnActiveGridSwitched);

	
	Super::NativeOnInitialized();

	StoreGrid_Equipment->SetOwningCanvas(GetCanvasPanel());
	StoreGrid_Consumable->SetOwningCanvas(GetCanvasPanel());
	StoreGrid_Crafting->SetOwningCanvas(GetCanvasPanel());

}

void UInventoryStoreWidgetSpatial::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateAllEquippedItemsStatus();

	UpdateInventoryGrids();
}

void UInventoryStoreWidgetSpatial::OnOpenedMenu()
{
	LOG_NETFUNCTIONCALL

	Super::OnOpenedMenu();

	PopulateItemsFromStore();
}

void UInventoryStoreWidgetSpatial::PopulateStore(UInventoryStoreComponent* Store)
{
	LOG_NETFUNCTIONCALL

	check(IsValid(Store));

	StoreGrid_Equipment->ConstructForStore(Store);
	StoreGrid_Consumable->ConstructForStore(Store);
	StoreGrid_Crafting->ConstructForStore(Store);

	StoreComponent = Store;
}

void UInventoryStoreWidgetSpatial::HandleOnActiveGridSwitched(UInventoryGridWidget* InventoryGridWidget)
{
	check(InventoryGridWidget && StoreGridSwitcher);
	FGameplayTag ItemCategory = InventoryGridWidget->GetItemCategory();
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment))
	{
		StoreGridSwitcher->SetActiveWidget(StoreGrid_Equipment);
	}
	else if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Consumable))
	{
		StoreGridSwitcher->SetActiveWidget(StoreGrid_Consumable);
	}
	else if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Crafting))
	{
		StoreGridSwitcher->SetActiveWidget(StoreGrid_Crafting);
	}
}

void UInventoryStoreWidgetSpatial::PopulateItemsFromStore()
{
	LOG_NETFUNCTIONCALL

	if (auto Store = GetStoreComponent())
	{
		///?
	}
}
