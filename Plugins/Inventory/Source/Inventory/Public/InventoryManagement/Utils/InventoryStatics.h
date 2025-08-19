// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Widgets/Utils/InventoryWidgetUtils.h"
#include "InventoryStatics.generated.h"

class UInventoryWidgetBase;
struct FInventoryItemManifest;
class UInventoryEquipmentComponent;
class UInventoryItemComponent;
class UInventoryComponent;
class UInventoryHoverItemWidget;
class UInventoryStoreComponent;

/**
 * 
 */
UCLASS()
class UInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryComponent* GetInventoryComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryEquipmentComponent* GetEquipmentComponent(const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryItemComponent* GetInventoryItemComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryStoreComponent* GetStoreComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API FGameplayTag GetItemCategory(UInventoryItemComponent* ItemComponent);

	template<typename T>
	static void ForEach2D(TArray<T>& Array, const int32 StartIndex, const FIntPoint& Range2D, const int32 GridColumns,
		TFunctionRef<void (typename TArray<T>::ElementType&)> Callback)
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

	template<typename T>
	static void ForEach2D(const TArray<T>& Array, const int32 StartIndex, const FIntPoint& Range2D, const int32 GridColumns,
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
	
	template<typename T>
	static void ForEach2DWithBreak(TArray<T>& Array, const int32 StartIndex, const FIntPoint& Range2D, const int32 GridColumns,
		TFunctionRef<bool (typename TArray<T>::ElementType&)> Callback)
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
					if (!Callback(Array[TileIndex]))
						return; // early exit
				}
			}
		}
	}

	template<typename T>
	static void ForEach2DWithBreak(const TArray<T>& Array, const int32 StartIndex, const FIntPoint& Range2D, const int32 GridColumns,
		TFunctionRef<bool (const typename TArray<T>::ElementType&)> Callback)
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
					if (!Callback(Array[TileIndex]))
						return; // early exit
				}
			}
		}
	}

	UFUNCTION(blueprintCallable, Category = "Inventory", BlueprintCosmetic)
	static void ItemHovered(APlayerController* PlayerController, UInventoryItem* Item);
	
	UFUNCTION(blueprintCallable, Category = "Inventory", BlueprintCosmetic)
	static void ItemUnhovered(APlayerController* PlayerController);

	UFUNCTION(BlueprintPure, Category = "Inventory", BlueprintCosmetic)
	static UInventoryHoverItemWidget* GetHoverItemWidget(const APlayerController* PlayerController);

	UFUNCTION(BlueprintPure, Category = "Inventory", BlueprintCosmetic)
	static bool IsHoverItemOwnedByPlayer(const APlayerController* PlayerController);

	// Returns widget of opened inventory menu (or Store menu)
	UFUNCTION(BlueprintPure, Category = "Inventory", BlueprintCosmetic)
	static UInventoryWidgetBase* GetInventoryWidget(const APlayerController* PlayerController);

	static bool CanEquipItem(const UInventoryItem* Item, const FGameplayTag& EquipmentTypeTag);
	static bool CanEquipItem(const FInventoryItemManifest& ItemManifest, const FGameplayTag& EquipmentTypeTag);

	static bool IsItemEquipable(const UInventoryItem* Item);

	static FGameplayTag GetItemEquipmentTag(const UInventoryItem* Item);

	static const FInventoryItemManifest& GetCoinItemManifest(const UObject* WorldContextObject);

	static int32 GetItemSellValue(const UInventoryItem* Item);
};
