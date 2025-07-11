// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryGridSlotWidget.h"
#include "InventoryEquippedGridSlot.generated.h"

class UInventoryEquipmentComponent;
struct FInventoryEquipmentSlot;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClickedSignature, UInventoryEquippedGridSlot*, GridSlot,
                                             const FGameplayTag&, EquipmentTypeTag);

/**
 * 
 */
UCLASS()
class UInventoryEquippedGridSlot : public UInventoryGridSlotWidgetBase//UInventoryGridSlotWidget
{
	GENERATED_BODY()
public:

	FEquippedGridSlotClickedSignature EquippedGridSlotClicked;

private:

	//UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems.Equipment"))
	//FGameplayTag EquipmentTypeTag;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.EquipmentSlots"))
	FGameplayTag EquipmentSlotTag;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UOverlay> Overlay_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_GrayedOutIcon;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryEquippedSlottedItemWidget> EquippedSlottedItemClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryEquippedSlottedItemWidget> EquippedSlottedItem;

	//TWeakObjectPtr<UInventoryItem> InventoryItem;

	//bool bIsAvailable = true;

	//FInventoryEquipmentSlot* BoundEquipmentSlot = nullptr;
	TWeakObjectPtr<UInventoryEquipmentComponent> EquipmentComponent;

	bool bPendingEquipping = false;
	TFunction<void ()> PendingEquippingFunction;
	
public:

	bool Bind(UInventoryEquipmentComponent* EquipComponent, const FGameplayTag& EquipSlotTag);

	//~ Begin of UUserWidget interface
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

	const FGameplayTag& GetEquipmentSlotTag() const {return EquipmentSlotTag;}
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
