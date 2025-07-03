// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryGridSlot.h"
#include "InventoryEquippedGridSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClickedSignature, UInventoryEquippedGridSlot*, GridSlot, const FGameplayTag&, EquipmentTypeTag);

/**
 * 
 */
UCLASS()
class UInventoryEquippedGridSlot : public UInventoryGridSlot
{
	GENERATED_BODY()
public:

	FEquippedGridSlotClickedSignature EquippedGridSlotClicked;

private:

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems.Equipment"))
	FGameplayTag EquipmentTypeTag;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UOverlay> Overlay_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_GrayedOutIcon;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryEquippedSlottedItem> EquippedSlottedItemClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryEquippedSlottedItem> EquippedSlottedItem;
	
public:

	//~ Begin of UUserWidget interface
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

	const FGameplayTag& GetEquipmentTypeTag() const { return EquipmentTypeTag; }

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetGrayedIconBrush(const FSlateBrush& Brush);

	UInventoryEquippedSlottedItem* OnItemEquipped(UInventoryItem* Item, const FGameplayTag& Tag, float TileSize);

	UInventoryEquippedSlottedItem* GetEquippedSlottedItem() const { return EquippedSlottedItem; }
	void SetEquippedSlottedItem(UInventoryEquippedSlottedItem* Item) {EquippedSlottedItem = Item;}
	void ClearEquippedSlot();
};
