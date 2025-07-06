// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryStorage.h"
#include "InventoryStorage_Grid.generated.h"

USTRUCT()
struct FInventoryStorageGrid
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FInventoryStorageGridSlot> GridSlots;

	UPROPERTY(EditAnywhere)
	int32 Rows = 1;

	UPROPERTY(EditAnywhere)
	int32 Columns = 1;
	
public:
	int32 GetIndexFromPosition(const FIntPoint& Position) const
	{
		return Position.X + Position.Y * Columns;
	}

	FIntPoint GetPositionFromIndex(int32 Index) const
	{
		return FIntPoint(Index % Columns, Index / Columns);
	}
	bool IsInGridBounds(int32 StartIndex, const FIntPoint& Dimensions) const;

	bool HasRoomAtIndex(const FInventoryStorageGridSlot& GridSlot, const FIntPoint& Dimensions,
							const TSet<int32>& CheckedIndexes, TSet<int32>& OutTentativelyClaimedIndexes,
							const int32 MaxStackSize, const FGameplayTag& ItemType) const;

	static bool CheckSlotConstraints(const FInventoryStorageGridSlot& GridSlot,
	                                 const FInventoryStorageGridSlot& CurGridSlot,
	                                 const TSet<int32>& CheckedIndexes,
	                                 TSet<int32>& OutTentativelyClaimedIndexes,
	                                 const int32 MaxStackSize,
	                                 const FGameplayTag& ItemType);
	int32 CalculateStackableFillAmountForSlot(int32 MaxStackSize, int32 Amount,
	                                          const FInventoryStorageGridSlot& GridSlot) const;
	int32 GetStackAmountInSlot(const FInventoryStorageGridSlot& GridSlot) const;
};

/**
 * 
 */
UCLASS(MinimalAPI)
class UInventoryStorage_Grid : public UInventoryStorage
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FGameplayTag, FInventoryStorageGrid> InventoryGrids;

public:


protected:

	bool HasGridForCategory(const FGameplayTag& ItemCategory) const;
	FInventoryStorageGrid& FindInventoryGridByCategory(const FGameplayTag& ItemCategory); 
	const FInventoryStorageGrid& FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const; 

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const override;

};
