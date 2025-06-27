// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidgetBase.generated.h"

UCLASS(MinimalAPI, Abstract)
class UInventoryWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:

	INVENTORY_API virtual FInventorySlotAvailabilityResult HasRoomForItem(class UInventoryItemComponent* ItemComponent) const
	{
		return FInventorySlotAvailabilityResult();
	}

	INVENTORY_API virtual void OnInventoryHovered(UInventoryItem* Item) {}
	INVENTORY_API virtual void OnInventoryUnhovered() {}
	INVENTORY_API virtual bool HasHoverItem() const { return false; }
	INVENTORY_API virtual class UInventoryHoverProxy* GetHoverItem() const { return nullptr; }
	INVENTORY_API virtual float GetTileSize() const { return 0.0f; }
	INVENTORY_API virtual void OnCloseMenu() {}
};
