// Copyright 4sandwiches


#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"

#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "Net/UnrealNetwork.h"

#include "DebugHelper.h"

namespace
{
	AActor* GetOutermostActor(const UObject* Object)
	{
		const UObject* Top = Object;
		for (;;)
		{
			UObject* CurrentOuter = Top->GetOuter();
			if (CurrentOuter->IsA<AActor>())
			{
				return Cast<AActor>(CurrentOuter);
			}
			if (CurrentOuter->IsA<UPackage>())
			{
				return nullptr;
			}
			Top = CurrentOuter;
		}

	}
}

UInventoryStorageGrid::UInventoryStorageGrid()
	: GridSlots(this)
{
}

void UInventoryStorageGrid::ConstructGrid()
{
	check(Rows > 0 && Columns > 0);

	if (!OwningActor.IsValid() || !InventoryComponent.IsValid())
	{
		InitOwner();
	}
	
	GridSlots.AddNumberSlots(Rows * Columns);
}

void UInventoryStorageGrid::InitOwner() const
{
	OwningActor = GetOutermostActor(this);
	InventoryComponent = UInventoryStatics::GetInventoryComponent(OwningActor.Get());

	InventoryComponent->OnItemAdded.AddDynamic(this, &UInventoryStorageGrid::HandleItemAdded);
	InventoryComponent->OnStackChanged.AddDynamic(this, &UInventoryStorageGrid::HandleStackChanged);
}

AActor* UInventoryStorageGrid::GetOwningActor() const
{
	if (!OwningActor.IsValid())
	{
		InitOwner();
		
	}
	return OwningActor.Get();
}

const FInventoryStorageGridSlot& UInventoryStorageGrid::GetGridSlot(int32 GridIndex) const
{
	check(GridSlots.IsValidIndex(GridIndex));
	return GridSlots.GetSlot(GridIndex);
}

const FInventoryStorageGridSlot& UInventoryStorageGrid::GetGridSlot(const FIntPoint& Position) const
{
	const int32 Index = GetIndexFromPosition(Position);
	return GetGridSlot(Index);
}

bool UInventoryStorageGrid::ContainsItem(UInventoryItem* Item) const
{
	return INDEX_NONE != GetItemIndex(Item);
}

int32 UInventoryStorageGrid::GetItemIndex(UInventoryItem* Item) const
{
	const int32 Index = GridSlots.Entries.IndexOfByPredicate([Item](const FInventoryStorageGridSlotEntry& GridSlotEntry)
	{
		const FInventoryStorageGridSlot& GridSlot = GridSlotEntry.Data;
		return GridSlot.GetStartIndex() == GridSlot.GetTileIndex() && GridSlot.GetInventoryItem() == Item;
	});
	return Index;
}

FInventorySlotAvailabilityResult UInventoryStorageGrid::HasRoomForItem(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
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

	// For each Grid Slot:
	for (const auto& GridSlot : GridSlots.GetAllSlots())
	{
		// If we don't have anymore to fill, break out of the loop early.
		if (AmountToFill <= 0)
			break;
		
		// Is this index claimed yet?
		if (CheckedIndexes.Contains(GridSlot.GetTileIndex()))
			continue;
		
		// Can the item fit here? (i.e. is it out of grid bounds?)
		if (!IsInGridBounds(GridSlot.GetTileIndex(), Dimensions))
			continue;

		// Is there room at this index? (i.e. are there other items in the way?)
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, Dimensions, CheckedIndexes, TentativelyClaimed, MaxStackSize, ItemManifest.GetItemType()))
			continue;
		
		// How much to fill?
		const int32 AmountToFillInSlot =
			Result.bStackable ? CalculateStackableFillAmountForSlot(MaxStackSize, AmountToFill, GridSlot) : 1;
		
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

void UInventoryStorageGrid::ConstructGrid(int32 InNumRows, int32 InNumColumns)
{
	Rows = InNumRows;
	Columns = InNumColumns;
	ConstructGrid();
}

void UInventoryStorageGrid::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryStorageGrid, GridSlots);
}

bool UInventoryStorageGrid::IsInGridBounds(int32 StartIndex, const FIntPoint& Dimensions) const
{
	if (!GridSlots.IsValidIndex(StartIndex))
		return false;
	const int32 EndColumn = StartIndex % Columns + Dimensions.X;
	const int32 EndRow = StartIndex / Columns + Dimensions.Y;

	return EndColumn <= Columns && EndRow <= Rows;
}

bool UInventoryStorageGrid::HasRoomAtIndex(const FInventoryStorageGridSlot& GridSlot,
	const FIntPoint& Dimensions, const TSet<int32>& CheckedIndexes, TSet<int32>& OutTentativelyClaimedIndexes,
	const int32 MaxStackSize, const FGameplayTag& ItemType) const
{
	bool bHasRoomAtIndex = true;

	UInventoryStatics::ForEach2D(GridSlots.Entries, GridSlot.GetTileIndex(), Dimensions, Columns, [&](const FInventoryStorageGridSlotEntry& GridSlotEntry)
	{
		const FInventoryStorageGridSlot& CurGridSlot = GridSlotEntry.Data;
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

bool UInventoryStorageGrid::CheckSlotConstraints(const FInventoryStorageGridSlot& GridSlot,
	const FInventoryStorageGridSlot& CurGridSlot, const TSet<int32>& CheckedIndexes,
	TSet<int32>& OutTentativelyClaimedIndexes, const int32 MaxStackSize, const FGameplayTag& ItemType)
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

int32 UInventoryStorageGrid::CalculateStackableFillAmountForSlot(int32 MaxStackSize, int32 Amount, const FInventoryStorageGridSlot& GridSlot) const
{
	const int32 RooInSlot = MaxStackSize - GetStackAmountInSlot(GridSlot);
	return FMath::Min(Amount, RooInSlot);
}

int32 UInventoryStorageGrid::GetStackAmountInSlot(const FInventoryStorageGridSlot& GridSlot) const
{
	int32 StackCount = GridSlot.GetStackCount();
	if (const int32 UpperLeftIndex = GridSlot.GetStartIndex(); UpperLeftIndex != INDEX_NONE)
	{
		StackCount = GridSlots.GetStackCount(UpperLeftIndex);
	}
	return StackCount;
}

void UInventoryStorageGrid::HandleItemAdded(UInventoryItem* Item)
{
	if (!IsValid(Item) || !MatchesCategory(Item))
		return;
	
	LOG_NETFUNCTIONCALL_MSG(TEXT("Adding item: [%s]"), *GetInventoryItemId(Item))

	auto Result = HasRoomForItem(Item->GetItemManifest());
	AddItemToIndexes(Result, Item);
	Result.Item = Item;
	OnItemAdded.Broadcast(Result);
}

void UInventoryStorageGrid::HandleStackChanged(const FInventorySlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get()))
		return;

	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			GridSlots.AddStackCount(Availability.Index, Availability.Amount);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.Amount);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.Amount);
		}
	}

	OnStackChanged.Broadcast(Result);
}

bool UInventoryStorageGrid::MatchesCategory(UInventoryItem* Item)
{
	return Item->GetItemManifest().GetItemCategory().MatchesTag(ItemCategory);
}

void UInventoryStorageGrid::AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.Amount);
	}
}

void UInventoryStorageGrid::AddItemAtIndex(UInventoryItem* Item, int32 Index, bool bStackable, int32 StackAmount)
{
}

void UInventoryStorageGrid::UpdateGridSlots(UInventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount)
{
	check(GridSlots.Entries.IsValidIndex(Index));

	if (bStackable)
	{
		GridSlots.SetStackCount(Index, StackAmount);
	}

	const FIntPoint Dimensions = UInventoryWidgetUtils::GetGridDimensionsOfItem(NewItem);

	TArray<int32> UpdatedSlots;
	UpdatedSlots.Reserve(Dimensions.X * Dimensions.Y);

	UInventoryStatics::ForEach2D(GridSlots.Entries, Index, Dimensions, GetColumns(),
		[&](const FInventoryStorageGridSlotEntry& GridSlotEntry)
		{
			const int32 GridIndex = GridSlotEntry.Data.GetTileIndex();
			GridSlots.SetInventoryItem(GridIndex, NewItem, Index);
			UpdatedSlots.Add(GridIndex);
		});
	
	OnGridSlotsUpdated.Broadcast(UpdatedSlots);
}

void UInventoryStorageGrid::RemoveItemFromGrid(UInventoryItem* ItemToRemove, const int32 GridIndex)
{
	const FIntPoint Dimensions = UInventoryWidgetUtils::GetGridDimensionsOfItem(ItemToRemove);
	TArray<int32> UpdatedSlots;
	UpdatedSlots.Reserve(Dimensions.X * Dimensions.Y);

	UInventoryStatics::ForEach2D(GridSlots.Entries, GridIndex, Dimensions, GetColumns(), [&](const FInventoryStorageGridSlotEntry& GridSlotEntry)
	{
		const int32 TileIndex = GridSlotEntry.Data.GetTileIndex();
		UpdatedSlots.Add(TileIndex);
		GridSlots.ClearSlot(TileIndex);
	});

	OnGridSlotsReset.Broadcast(UpdatedSlots);
}

void UInventoryStorageGrid::OnRep_GridSlots()
{
	LOG_NETFUNCTIONCALL
}

void UInventoryStorageGrid::NotifyGridChanged(TArrayView<FPlatformTypes::int32> ArrayView)
{
	LOG_NETFUNCTIONCALL

	//OnGridSlotsUpdated.Broadcast(ArrayView);
}

void UInventoryStorageGrid::SetStackCount(int32 GridIndex, int32 NewStackCount)
{
	GridSlots.SetStackCount(GridIndex, NewStackCount);
}
