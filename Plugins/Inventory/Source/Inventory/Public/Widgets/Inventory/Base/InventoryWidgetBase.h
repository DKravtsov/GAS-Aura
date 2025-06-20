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
};
