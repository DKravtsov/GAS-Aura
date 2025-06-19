// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "InventoryWidgetSpatial.generated.h"

class UWidgetSwitcher;
class UInventoryGrid;

UCLASS(MinimalAPI, Abstract)
class UInventoryWidgetSpatial : public UInventoryWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGrid> InventoryGrid_Equipment;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGrid> InventoryGrid_Consumable;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGrid> InventoryGrid_Crafting;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> GridSwitcher;
};
