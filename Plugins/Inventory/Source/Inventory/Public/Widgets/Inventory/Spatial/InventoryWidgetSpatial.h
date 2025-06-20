// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "InventoryWidgetSpatial.generated.h"

class UButton;
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

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Equipment;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Consumable;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Crafting;

public:

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
	//~ End of UUserWidget interface

	//~ Begin of UInventoryWidgetBase interface
	virtual FInventorySlotAvailabilityResult HasRoomForItem(class UInventoryItemComponent* ItemComponent) const override;
	//~ End of UInventoryWidgetBase interface
private:

	UFUNCTION()
	void ShowEquipmentGrid();

	UFUNCTION()
	void ShowConsumableGrid();

	UFUNCTION()
	void ShowCraftingGrid();

	void DisableButton(UButton* Button);
	void SetActiveGrid(UInventoryGrid* Grid, UButton* Button);
};
