// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryStoreComponent.h"
#include "InventoryLootContainerComponent.generated.h"


UCLASS(MinimalAPI, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent), Blueprintable)
class UInventoryLootContainerComponent : public UInventoryStoreComponent
{
	GENERATED_BODY()

public:

	//~ Begin of UInventoryStoreComponent interface
	virtual int32 GetSellValue(const UInventoryItem* Item, int32 StackCount) const override;
	virtual int32 GetPurchaseValue(const UInventoryItem* Item, int32 StackCount) const override;
	//~ End of UInventoryStoreComponent interface

};
