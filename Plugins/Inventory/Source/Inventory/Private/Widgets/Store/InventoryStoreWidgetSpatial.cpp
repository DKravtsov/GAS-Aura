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

void UInventoryStoreWidgetSpatial::OnCloseMenu()
{
	LOG_NETFUNCTIONCALL

	Super::OnCloseMenu();

	if (ActiveStoreGrid.IsValid())
	{
		ActiveStoreGrid->OnHide();
	}
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

void UInventoryStoreWidgetSpatial::UpdateInventoryGrids()
{
	Super::UpdateInventoryGrids();

	TArray<int32> AllTiles;
	AllTiles.AddUninitialized(StoreGrid_Equipment->GetNumGridSlots());
	for (int32 Index = 0; Index < AllTiles.Num(); ++Index)
	{
		AllTiles[Index] = Index;
	}

	const TArrayView<int32> AllTilesView(AllTiles);
	StoreGrid_Equipment->HandleOnUpdateGridSlots(AllTilesView);
	StoreGrid_Consumable->HandleOnUpdateGridSlots(AllTilesView);
	StoreGrid_Crafting->HandleOnUpdateGridSlots(AllTilesView);

	StoreGrid_Equipment->UpdateInventoryGridSlots();
	StoreGrid_Consumable->UpdateInventoryGridSlots();
	StoreGrid_Crafting->UpdateInventoryGridSlots();
}

void UInventoryStoreWidgetSpatial::HandleOnActiveGridSwitched(UInventoryGridWidget* InventoryGridWidget)
{
	check(InventoryGridWidget && StoreGridSwitcher);
	FGameplayTag ItemCategory = InventoryGridWidget->GetItemCategory();

	if (ActiveStoreGrid.IsValid())
	{
		ActiveStoreGrid->OnHide();
	}
	
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment))
	{
		ActiveStoreGrid = StoreGrid_Equipment;
	}
	else if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Consumable))
	{
		ActiveStoreGrid = StoreGrid_Consumable;
	}
	else if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Crafting))
	{
		ActiveStoreGrid = StoreGrid_Crafting;
	}
	else
	{
		checkNoEntry();
	}
	StoreGridSwitcher->SetActiveWidget(ActiveStoreGrid.Get());
}

void UInventoryStoreWidgetSpatial::PopulateItemsFromStore()
{
	LOG_NETFUNCTIONCALL

	if (auto Store = GetStoreComponent())
	{
		UpdateInventoryGrids();
	}
}
