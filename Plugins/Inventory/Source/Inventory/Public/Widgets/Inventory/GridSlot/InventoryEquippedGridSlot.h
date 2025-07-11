// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryGridSlotWidget.h"
#include "InventoryGridTypes.h"
#include "InventoryEquippedGridSlot.generated.h"

class UInventoryEquipmentComponent;
struct FInventoryEquipmentSlot;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClickedSignature, UInventoryEquippedGridSlot*, GridSlot,
                                             const FGameplayTag&, EquipmentTypeTag);

/**
 * 
 */
UCLASS()
class UInventoryEquippedGridSlot : public UInventoryGridSlotWidgetBase
{
	GENERATED_BODY()
public:

	FEquippedGridSlotClickedSignature EquippedGridSlotClicked;

private:

	// UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.EquipmentSlots"))
	// FGameplayTag EquipmentSlotTag;

	UPROPERTY(EditAnywhere, Category="Inventory")
	EInventoryEquipmentSlot EquipmentSlotId = EInventoryEquipmentSlot::Invalid;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UOverlay> Overlay_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_GrayedOutIcon;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryEquippedSlottedItemWidget> EquippedSlottedItemClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryEquippedSlottedItemWidget> EquippedSlottedItem;

	TWeakObjectPtr<UInventoryEquipmentComponent> EquipmentComponent;

	TFunction<void ()> PendingEquippingFunction;
	bool bPendingEquipping = false;
	
public:

	bool Bind(UInventoryEquipmentComponent* EquipComponent, EInventoryEquipmentSlot SlotId);

	//~ Begin of UUserWidget interface
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

	//const FGameplayTag& GetEquipmentSlotTag() const {return EquipmentSlotTag;}
	EInventoryEquipmentSlot GetSlotId() const { return EquipmentSlotId; }
	
	const FGameplayTag& GetEquipmentTypeTag() const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetGrayedIconBrush(const FSlateBrush& Brush);

	bool IsAvailable() const;

	TWeakObjectPtr<UInventoryItem> GetInventoryItem() const;
	UInventoryEquippedSlottedItemWidget* OnItemEquipped(UInventoryItem* Item, const FGameplayTag& Tag, float TileSize);

	UInventoryEquippedSlottedItemWidget* GetEquippedSlottedItem() const { return EquippedSlottedItem; }
	void SetEquippedSlottedItem(UInventoryEquippedSlottedItemWidget* Item) {EquippedSlottedItem = Item;}
	void ClearEquippedSlot();

	void UpdateIfPending();

private:
	const FInventoryEquipmentSlot* GetBoundEquipmentSlot() const;
	void SetInventoryItem(UInventoryItem* Item);
};
