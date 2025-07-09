// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "InventoryWidgetSpatial.generated.h"

class UInventoryEquippedSlottedItemWidget;
class UButton;
class UWidgetSwitcher;
class UInventoryGridWidget;

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
	TObjectPtr<UInventoryGridWidget> InventoryGrid_Equipment;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> InventoryGrid_Consumable;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> InventoryGrid_Crafting;

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

	TWeakObjectPtr<UInventoryGridWidget> ActiveGrid;

	FTimerHandle TimerHandle_Description;

public:

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
	INVENTORY_API virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

	//~ Begin of UInventoryWidgetBase interface
	virtual void OnInventoryHovered(UInventoryItem* Item) override;
	virtual void OnInventoryUnhovered() override;
	virtual bool HasHoverItem() const override;
	virtual UInventoryHoverItemWidget* GetHoverItem() const override;
	virtual float GetTileSize() const override;
	virtual void OnCloseMenu() override;
	virtual void OnOpenedMenu() override;
	// virtual bool IsItemEquipped(const UInventoryItem* Item) const override;
	// virtual bool CanEquipItem(const UInventoryItem* Item) const override;
	// virtual class UInventoryEquippedSlottedItemWidget* FindEquippedSlottedItemFor(const UInventoryItem* Item) const override;
	// virtual UInventoryItem* GetItemInSlot(const FGameplayTag& EquipmentTypeTag) const override;
protected:
	//virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const override;;
	//~ End of UInventoryWidgetBase interface

public:

	virtual bool TryEquipItem(UInventoryItem* Item, const FGameplayTag& EquipmentTypeTag, bool bAlwaysEquip = false, UInventoryItem** PreviousEquippedItem = nullptr) override;
	
private:

	UFUNCTION()
	void ShowEquipmentGrid();

	UFUNCTION()
	void ShowConsumableGrid();

	UFUNCTION()
	void ShowCraftingGrid();

	UFUNCTION()
	void EquippedGridSlotClicked(UInventoryEquippedGridSlot* GridSlot, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION()
	void EquippedSlottedItemClicked(UInventoryEquippedSlottedItemWidget* EquippedSlottedItem);

	void DisableButton(UButton* Button);
	void SetActiveGrid(UInventoryGridWidget* Grid, UButton* Button);

	UInventoryGridWidget* GetInventoryGridByCategory(const FGameplayTag& ItemCategory) const;

	UInventoryItemDescription* GetOrCreateItemDescription();

	bool CanEquipHoverItem(const UInventoryEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const;

	UInventoryEquippedGridSlot* FindSlotWithEquippedItem(const UInventoryItem* EquippedItem) const;
	void ClearSlotOfItem(UInventoryEquippedGridSlot* EquippedGridSlot);
	void RemoveEquippedSlottedItem(UInventoryEquippedSlottedItemWidget* EquippedSlottedItem);
	void MakeEquippedSlottedItem(const UInventoryEquippedSlottedItemWidget* EquippedSlottedItem, UInventoryEquippedGridSlot* EquippedGridSlot, UInventoryItem* ItemToEquip);

	void BroadcastClickedDelegates(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip) const;

	UInventoryEquippedGridSlot* FindEquippedGridSlotByType(const FGameplayTag& EquipmentTypeTag) const;

	FGameplayTag FindItemBestEquipType(const UInventoryItem* Item) const;
};
