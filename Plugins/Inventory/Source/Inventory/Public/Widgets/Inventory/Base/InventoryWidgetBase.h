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

	INVENTORY_API virtual void OnInventoryHovered(UInventoryItem* Item) {}
	INVENTORY_API virtual void OnInventoryUnhovered() {}
	INVENTORY_API virtual bool HasHoverItem() const { return false; }
	INVENTORY_API virtual class UInventoryHoverItemWidget* GetHoverItem() const { return nullptr; }
	INVENTORY_API virtual float GetTileSize() const { return 0.0f; }
	INVENTORY_API virtual void OnOpenedMenu() {}
	INVENTORY_API virtual void OnCloseMenu() {}


};
