// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryHighlightableInterface.generated.h"


UINTERFACE()
class UInventoryHighlightableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INVENTORY_API IInventoryHighlightableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void Highlight();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void UnHighlight();
};
