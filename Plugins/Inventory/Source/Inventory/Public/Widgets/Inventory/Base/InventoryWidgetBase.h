// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "Blueprint/UserWidget.h"
#include "Items/Components/InventoryItemComponent.h"
#include "InventoryWidgetBase.generated.h"

UCLASS(MinimalAPI, Abstract)
class UInventoryWidgetBase : public UUserWidget
{
	GENERATED_BODY()

	TWeakObjectPtr<UObject> ContextObject;
	
public:

	void SetContextObject(UObject* NewContextObject) {ContextObject = NewContextObject;}

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UObject* GetContextObject() const {return ContextObject.Get();}

	// FInventorySlotAvailabilityResult HasRoomForItem(const UInventoryItemComponent* ItemComponent) const
	// {
	// 	return HasRoomForItemInternal(ItemComponent->GetItemManifest(), -1);
	// }
	//
	// FInventorySlotAvailabilityResult HasRoomForItem(const struct FInventoryItemManifest& ItemManifest, const int32 StackCountOverride = -1) const
	// {
	// 	return HasRoomForItemInternal(ItemManifest, StackCountOverride);
	// }

	INVENTORY_API virtual void OnInventoryHovered(UInventoryItem* Item) {}
	INVENTORY_API virtual void OnInventoryUnhovered() {}
	INVENTORY_API virtual bool HasHoverItem() const { return false; }
	INVENTORY_API virtual class UInventoryHoverProxyWidget* GetHoverItem() const { return nullptr; }
	INVENTORY_API virtual float GetTileSize() const { return 0.0f; }
	INVENTORY_API virtual void OnOpenedMenu() {}
	INVENTORY_API virtual void OnCloseMenu() {}

	//INVENTORY_API virtual bool IsItemEquipped(const UInventoryItem* Item) const { return false; }
	//INVENTORY_API virtual bool CanEquipItem(const UInventoryItem* Item) const { return false; }
	//INVENTORY_API virtual class UInventoryEquippedSlottedItemWidget* FindEquippedSlottedItemFor(const UInventoryItem* Item) const { return nullptr; }
	//INVENTORY_API virtual UInventoryItem* GetItemInSlot(const FGameplayTag& EquipmentTypeTag) const { return nullptr; }

	INVENTORY_API virtual bool TryEquipItem(UInventoryItem* Item, const FGameplayTag& EquipmentTypeTag, bool bAlwaysEquip = false, UInventoryItem** PreviousEquippedItem = nullptr) { return false; }
protected:

	// virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
	// {
	// 	return FInventorySlotAvailabilityResult();
	// }
	
};
