// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryWidgetUtils.generated.h"

/**
 * 
 */
UCLASS()
class UInventoryWidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	INVENTORY_API static int32 GetIndexFromPosition(const FIntPoint& Position, int32 Columns);
};
