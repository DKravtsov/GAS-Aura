// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "UObject/Object.h"
#include "InventorySpatialStorageGrid.generated.h"

struct FInventoryItemManifest;
/**
 * 
 */
UCLASS(MinimalAPI)
class UInventorySpatialStorageGrid : public UObject
{
	GENERATED_BODY()

	//~ todo: Replace with FastArray
	UPROPERTY(Replicated)
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

	FInventorySlotAvailabilityResult HasRoomForItem(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const;
	
	void ConstructGrid(int32 InNumRows, int32 InNumColumns);

	//~ Begin of UObject interface
	virtual bool IsSupportedForNetworking() const override {return true;}
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End of UObject interface
	
private:
	
	void ConstructGrid();
	
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
