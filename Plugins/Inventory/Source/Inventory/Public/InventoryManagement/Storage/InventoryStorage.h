// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryGridTypes.h"
#include "InventoryStorage.generated.h"

class UInventoryItemComponent;

/**
 * 
 */
UCLASS(MinimalAPI, Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class UInventoryStorage : public UObject
{
	GENERATED_BODY()

public:

	bool HasRoomForItem(FInventorySlotAvailabilityResult& Result, const UInventoryItemComponent* ItemComponent) const;
	bool HasRoomForItem(FInventorySlotAvailabilityResult& Result, const struct FInventoryItemManifest& ItemManifest, const int32 StackCountOverride = -1) const;

	virtual void SetupStorage() {}

protected:

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const;
};
