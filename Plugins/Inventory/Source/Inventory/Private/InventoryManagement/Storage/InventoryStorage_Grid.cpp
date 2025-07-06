// Copyright 4sandwiches


#include "InventoryManagement/Storage/InventoryStorage_Grid.h"

#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Items/Manifest/InventoryItemManifest.h"

bool UInventoryStorage_Grid::HasGridForCategory(const FGameplayTag& ItemCategory) const
{
	return InventoryGrids.Contains(ItemCategory);
}

FInventoryStorageGrid& UInventoryStorage_Grid::FindInventoryGridByCategory(const FGameplayTag& ItemCategory)
{
	return InventoryGrids.FindChecked(ItemCategory);
}

const FInventoryStorageGrid& UInventoryStorage_Grid::FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const
{
	return InventoryGrids.FindChecked(ItemCategory);
}

FInventorySlotAvailabilityResult UInventoryStorage_Grid::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	FInventorySlotAvailabilityResult Result;

	// Determine if the item is stackable.Add commentMore actions
	const auto StackableFragment = ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	const auto GridFragment = ItemManifest.GetFragmentOfType<FInventoryItemGridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint{1,1};
	
	// Determine how many stacks to add.

	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? (StackCountOverride >= 0 ? StackCountOverride : StackableFragment->GetStackCount()) : 1;

	TSet<int32> CheckedIndexes;

	const auto& Grid = FindInventoryGridByCategory(ItemManifest.GetItemCategory());
	
	// For each Grid Slot:
	for (const auto& GridSlot : Grid.GridSlots)
	{
		// If we don't have anymore to fill, break out of the loop early.
		if (AmountToFill <= 0)
			break;
		
		// Is this index claimed yet?
		if (CheckedIndexes.Contains(GridSlot.GetTileIndex()))
			continue;
		
		// Can the item fit here? (i.e. is it out of grid bounds?)
		if (!Grid.IsInGridBounds(GridSlot.GetTileIndex(), Dimensions))
			continue;

		// Is there room at this index? (i.e. are there other items in the way?)
		TSet<int32> TentativelyClaimed;
		if (!Grid.HasRoomAtIndex(GridSlot, Dimensions, CheckedIndexes, TentativelyClaimed, MaxStackSize, ItemManifest.GetItemType()))
			continue;
		
		// How much to fill?
		const int32 AmountToFillInSlot =
			Result.bStackable ? Grid.CalculateStackableFillAmountForSlot(MaxStackSize, AmountToFill, GridSlot) : 1;
		
		if (AmountToFillInSlot <= 0)
			continue;
		
		CheckedIndexes.Append(TentativelyClaimed);
		
		// Update the amount left to fillAdd commentMore actions
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			GridSlot.GetInventoryItem().IsValid() ? GridSlot.GetStartIndex() : GridSlot.GetTileIndex(),
			Result.bStackable ? AmountToFillInSlot : 0,
			GridSlot.GetInventoryItem().IsValid()
			);

		AmountToFill -= AmountToFillInSlot;
		// How much is the Remainder?
		Result.Remainder = AmountToFill;
	}
	
	return Result;
}

bool FInventoryStorageGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& Dimensions) const
{
	if (!GridSlots.IsValidIndex(StartIndex))
		return false;
	const int32 EndColumn = StartIndex % Columns + Dimensions.X;
	const int32 EndRow = StartIndex / Columns + Dimensions.Y;

	return EndColumn <= Columns && EndRow <= Rows;
}

bool FInventoryStorageGrid::HasRoomAtIndex(const FInventoryStorageGridSlot& GridSlot, const FIntPoint& Dimensions,
						const TSet<int32>& CheckedIndexes, TSet<int32>& OutTentativelyClaimedIndexes,
						const int32 MaxStackSize, const FGameplayTag& ItemType) const
{
	bool bHasRoomAtIndex = true;

	UInventoryStatics::ForEach2D(GridSlots, GridSlot.GetTileIndex(), Dimensions, Columns, [&](const FInventoryStorageGridSlot& CurGridSlot)
	{
		if (CheckSlotConstraints(GridSlot, CurGridSlot, CheckedIndexes, OutTentativelyClaimedIndexes, MaxStackSize, ItemType))
		{
			OutTentativelyClaimedIndexes.Add(CurGridSlot.GetTileIndex());
			return true;
		}
		bHasRoomAtIndex = false;
		return false;
	});
	
	return bHasRoomAtIndex;
}

bool FInventoryStorageGrid::CheckSlotConstraints(const FInventoryStorageGridSlot& GridSlot,
										  const FInventoryStorageGridSlot& CurGridSlot,
										  const TSet<int32>& CheckedIndexes,
										  TSet<int32>& OutTentativelyClaimedIndexes,
										  const int32 MaxStackSize,
										  const FGameplayTag& ItemType)
{

	// Index claimed?
	if (CheckedIndexes.Contains(CurGridSlot.GetTileIndex()))
		return false;
	
	// Has valid item?
	if(!CurGridSlot.GetInventoryItem().IsValid())
	{
		OutTentativelyClaimedIndexes.Add(CurGridSlot.GetTileIndex());
		return true;
	}

	// Is this grid slot an upper left slot? (we care only about upper left slot because only there our stack stored)
	if (CurGridSlot.GetStartIndex() != GridSlot.GetTileIndex())
		return false; 
	
	// If so, is this a stackable item?
	const UInventoryItem* CurItem = CurGridSlot.GetInventoryItem().Get();
	if (!CurItem->IsStackable())
		return false;
	
	// Is this item the same type as the item we're trying to add?
	if (!CurItem->GetItemType().MatchesTagExact(ItemType))
		return false;

	// If stackable, is this slot at the max stack size already?
	if (GridSlot.GetStackCount() >= MaxStackSize)
		return false;
	
	return true;
}

int32 FInventoryStorageGrid::CalculateStackableFillAmountForSlot(const int32 MaxStackSize,
														  const int32 Amount, const FInventoryStorageGridSlot& GridSlot) const
{
	const int32 RooInSlot = MaxStackSize - GetStackAmountInSlot(GridSlot);
	return FMath::Min(Amount, RooInSlot);
}

int32 FInventoryStorageGrid::GetStackAmountInSlot(const FInventoryStorageGridSlot& GridSlot) const
{
	int32 StackCount = GridSlot.GetStackCount();
	if (const int32 UpperLeftIndex = GridSlot.GetStartIndex(); UpperLeftIndex != INDEX_NONE)
	{
		const FInventoryStorageGridSlot& UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		StackCount = UpperLeftGridSlot.GetStackCount();
	}
	return StackCount;
}
