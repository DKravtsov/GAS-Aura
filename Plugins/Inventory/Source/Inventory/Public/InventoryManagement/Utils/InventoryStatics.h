// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Widgets/Utils/InventoryWidgetUtils.h"
#include "InventoryStatics.generated.h"

class UInventoryItemComponent;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class UInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryItemComponent* GetInventoryItemComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API FGameplayTag GetItemCategory(UInventoryItemComponent* ItemComponent);

	template<typename T>
	static void ForEach2D(TArray<T>& Array, const int32 StartIndex, const FIntPoint& Range2D, const int32 GridColumns,
		TFunctionRef<void (const typename TArray<T>::ElementType&)> Callback)
	{
		const FIntPoint StartPos = UInventoryWidgetUtils::GetPositionFromIndex(StartIndex, GridColumns); 
		for (int32 RowIndex = 0; RowIndex < Range2D.Y; ++RowIndex)
		{
			for (int32 ColIndex = 0; ColIndex < Range2D.X; ++ColIndex)
			{
				const FIntPoint Pos = StartPos + FIntPoint(ColIndex, RowIndex);
				const int32 TileIndex = UInventoryWidgetUtils::GetIndexFromPosition(Pos, GridColumns);
				if (Array.IsValidIndex(TileIndex))
				{
					Callback(Array[TileIndex]);
				}
			}
		}
	}
};
