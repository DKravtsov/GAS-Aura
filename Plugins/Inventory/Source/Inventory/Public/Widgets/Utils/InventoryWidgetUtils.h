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

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static FVector2D GetWidgetPosition(class UWidget* Widget);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static FVector2D GetWidgetSize(const UWidget* Widget);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static bool IsWithinBounds(const FVector2D& WidgetPosition, const FVector2D& WidgetSize, const FVector2D& MousePosition);
};
