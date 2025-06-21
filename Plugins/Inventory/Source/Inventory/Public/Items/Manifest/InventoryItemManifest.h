// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "InventoryItemManifest.generated.h"

/**
 * Contains all the necessary data for creating a new Inventory item
 */
USTRUCT(BlueprintType)
struct FInventoryItemManifest
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.ItemCategory"))
	FGameplayTag ItemCategory;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems"))
	FGameplayTag ItemType;
	
public:
	INVENTORY_API FInventoryItemManifest();

	FGameplayTag GetItemCategory() const {return ItemCategory;}
	FGameplayTag GetItemType() const {return ItemType;}

	INVENTORY_API class UInventoryItem* Manifest(UObject* NewOuter);
};
