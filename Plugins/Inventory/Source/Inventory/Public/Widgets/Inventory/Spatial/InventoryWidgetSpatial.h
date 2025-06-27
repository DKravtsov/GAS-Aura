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

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<class UInventoryItemDescription> ItemDescriptionClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryItemDescription> ItemDescription;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionDelay = 0.2f;

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

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UInventoryEquippedGridSlot>> EquippedGridSlots;

	TWeakObjectPtr<UInventoryGrid> ActiveGrid;

	FTimerHandle TimerHandle_Description;

public:

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
	INVENTORY_API virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

	//~ Begin of UInventoryWidgetBase interface
	virtual FInventorySlotAvailabilityResult HasRoomForItem(class UInventoryItemComponent* ItemComponent) const override;
	virtual void OnInventoryHovered(UInventoryItem* Item) override;
	virtual void OnInventoryUnhovered() override;
	virtual bool HasHoverItem() const override;
	virtual UInventoryHoverProxy* GetHoverItem() const override;
	//~ End of UInventoryWidgetBase interface
private:

	UFUNCTION()
	void ShowEquipmentGrid();

	UFUNCTION()
	void ShowConsumableGrid();

	UFUNCTION()
	void ShowCraftingGrid();

	UFUNCTION()
	void EquippedGridSlotClicked(UInventoryEquippedGridSlot* GridSlot, const FGameplayTag& EquipmentTypeTag);

	void DisableButton(UButton* Button);
	void SetActiveGrid(UInventoryGrid* Grid, UButton* Button);

	UInventoryGrid* GetInventoryGridByCategory(const FGameplayTag& ItemCategory) const;

	UInventoryItemDescription* GetOrCreateItemDescription();

	bool CanEquipHoverItem(UInventoryEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag);
};
