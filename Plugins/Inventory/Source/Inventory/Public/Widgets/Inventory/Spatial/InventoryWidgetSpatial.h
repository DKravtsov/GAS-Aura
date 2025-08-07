// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "InventoryWidgetSpatial.generated.h"

class UInventoryEquippedSlottedItemWidget;
class UButton;
class UWidgetSwitcher;
class UInventoryGridWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryGridWidgetSwitchedSignature, UInventoryGridWidget*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryCommonNotifySignature, FGameplayTag, ErrorTag);

UCLASS(MinimalAPI, Abstract)
class UInventoryWidgetSpatial : public UInventoryWidgetBase
{
	GENERATED_BODY()

public:

	FInventoryCommonNotifySignature OnInventoryInteractionError;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<class UInventoryItemDescription> ItemDescriptionClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryItemDescription> ItemDescription;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<class UInventoryItemDescription> EquippedItemDescriptionClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryItemDescription> EquippedItemDescription;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float EquippedDescriptionDelay = 0.2f;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> InventoryGrid_Equipment;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> InventoryGrid_Consumable;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> InventoryGrid_Crafting;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> GridSwitcher;

	FInventoryGridWidgetSwitchedSignature OnActiveGridSwitched;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Equipment;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Consumable;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Crafting;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UInventoryEquipmentSlotWidget>> EquippedGridSlots;

	TWeakObjectPtr<UInventoryGridWidget> ActiveGrid;

	FTimerHandle TimerHandle_Description;
	FTimerHandle TimerHandle_EquippedDescription;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryHoverItemWidget> HoverItemClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryHoverItemWidget> HoverItem;

	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;
	
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
	//~ End of UInventoryWidgetBase interface

	//~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface

	FInventoryGridWidgetSwitchedSignature& GetOnGridWidgetSwitchedDelegate() {return OnActiveGridSwitched;}

	bool IsHoverItemOwnedByPlayer() const;

protected:
	UCanvasPanel* GetCanvasPanel() const {return CanvasPanel;}
	
	UFUNCTION()
	void UpdateEquippedItemStatus(UInventoryItem* Item);

	void UpdateAllEquippedItemsStatus();

	virtual void UpdateInventoryGrids();

private:

	UFUNCTION()
	void ShowEquipmentGrid();

	UFUNCTION()
	void ShowConsumableGrid();

	UFUNCTION()
	void ShowCraftingGrid();

	UFUNCTION()
	void EquipmentSlotClicked(UInventoryEquipmentSlotWidget* GridSlot, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION()
	void EquippedSlottedItemClicked(UInventoryEquippedSlottedItemWidget* EquippedSlottedItem);

	void ShowEquippedItemDescription(UInventoryItem* Item);

	void DisableButton(UButton* Button);
	void SetActiveGrid(UInventoryGridWidget* Grid, UButton* Button);

	UInventoryGridWidget* GetInventoryGridByCategory(const FGameplayTag& ItemCategory) const;

	UInventoryItemDescription* GetOrCreateItemDescription();
	UInventoryItemDescription* GetOrCreateEquippedItemDescription();

	bool CanEquipHoverItem(const UInventoryEquipmentSlotWidget* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const;

	UInventoryEquipmentSlotWidget* FindSlotWithEquippedItem(const UInventoryItem* EquippedItem) const;
	void ClearSlotOfItem(UInventoryEquipmentSlotWidget* EquippedGridSlot);
	void RemoveEquippedSlottedItem(UInventoryEquippedSlottedItemWidget* EquippedSlottedItem);
	void MakeEquippedSlottedItem(const UInventoryEquippedSlottedItemWidget* EquippedSlottedItem, UInventoryEquipmentSlotWidget* EquippedGridSlot, UInventoryItem* ItemToEquip);

	void BroadcastClickedDelegates(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId) const;

	UInventoryEquipmentSlotWidget* FindEquippedGridSlotByType(const FGameplayTag& EquipmentTypeTag) const;
	UInventoryEquipmentSlotWidget* FindEquippedGridSlot(EInventoryEquipmentSlot SlotId) const;

	FGameplayTag FindItemBestEquipType(const UInventoryItem* Item) const;

	void ShowDefaultCursor() const;
	void ClearHoverItem();
	void HandleOnHoverItemReset();
	void HandleOnHoverItemUpdated(UInventoryItem* Item, bool bStackable, int32 StackCount, int32 PreviousIndex);

};
