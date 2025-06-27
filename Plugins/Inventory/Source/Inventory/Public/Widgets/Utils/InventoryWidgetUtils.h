// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryWidgetUtils.generated.h"

class UInventoryItem;
/**
 * 
 */
UCLASS()
class UInventoryWidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static INVENTORY_API int32 GetIndexFromPosition(const FIntPoint& Position, const int32 Columns);

	static INVENTORY_API FIntPoint GetPositionFromIndex(const int32 Index, const int32 Columns);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static INVENTORY_API FVector2D GetWidgetPosition(class UWidget* Widget);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static INVENTORY_API FVector2D GetWidgetSize(const UWidget* Widget);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static INVENTORY_API bool IsWithinBounds(const FVector2D& WidgetPosition, const FVector2D& WidgetSize, const FVector2D& MousePosition);
	
	static INVENTORY_API const FInventoryItemImageFragment* GetImageFragmentFromInventoryItem(const UInventoryItem* Item);
	static INVENTORY_API const FInventoryItemGridFragment* GetGridFragmentFromInventoryItem(const UInventoryItem* Item);

	static INVENTORY_API FIntPoint GetGridDimensionsOfItem(const UInventoryItem* Item);

	static INVENTORY_API FVector2D GetDrawSize(const FInventoryItemGridFragment& GridFragment, const float TileSize)
	{
		const float IconTileSize = TileSize - GridFragment.GetGridPadding() * 2; 
		return GridFragment.GetGridSize() * IconTileSize;
	}

	static INVENTORY_API FVector2D GetItemDrawSize(const UInventoryItem* Item, const float TileSize);

};
