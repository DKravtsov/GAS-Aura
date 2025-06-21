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

	static int32 GetIndexFromPosition(const FIntPoint& Position, const int32 Columns);

	static FIntPoint GetPositionFromIndex(const int32 Index, const int32 Columns);
};
