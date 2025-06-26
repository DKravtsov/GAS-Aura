// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Manifest/InventoryItemManifest.h"
#include "InventoryItemData.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UInventoryItemData : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FInventoryItemManifest ItemManifest;

public:

	const FInventoryItemManifest& GetItemManifest() const { return ItemManifest; }
};
