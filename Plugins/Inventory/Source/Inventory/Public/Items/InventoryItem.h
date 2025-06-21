// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class UInventoryItem : public UObject
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(BaseStruct="/Script/Inventory.InventoryItemManifest"), Replicated)
	FInstancedStruct ItemManifest;

public:

	void SetItemManifest(const FInventoryItemManifest& InItemManifest);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
