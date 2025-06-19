// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryHighlightableInterface.generated.h"


UINTERFACE(MinimalAPI)
class UInventoryHighlightableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IInventoryHighlightableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	INVENTORY_API void Highlight();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	INVENTORY_API void UnHighlight();
};
