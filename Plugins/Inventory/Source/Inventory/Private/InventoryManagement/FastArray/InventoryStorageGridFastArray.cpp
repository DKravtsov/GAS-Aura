// Copyright 4sandwiches


#include "InventoryManagement/FastArray/InventoryStorageGridFastArray.h"

#include "DebugHelper.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"


TArray<FInventoryStorageGridSlot> FInventoryStorageGridFastArray::GetAllSlots() const
{
	TArray<FInventoryStorageGridSlot> Result;
	Result.Reserve(Entries.Num());
	for (const auto& Entry : Entries)
	{
		Result.Add(Entry.Data);
	}
	return Result;
}

void FInventoryStorageGridFastArray::AddSlot(int32 InTileIndex)
{
	check(OwningGrid != nullptr);
	const AActor* OwningActor = OwningGrid->GetOwningActor();
	check(OwningActor->HasAuthority());

	auto& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Data.SetTileIndex(InTileIndex);
	MarkItemDirty(NewEntry);
}

void FInventoryStorageGridFastArray::AddNumberSlots(int32 Count)
{
	check(OwningGrid != nullptr);
	const AActor* OwningActor = OwningGrid->GetOwningActor();
	check(OwningActor->HasAuthority());

	const int32 StartIndex = Entries.Num();
	Entries.AddDefaulted(Count);
	for (int32 Index = 0; Index < Count; Index++)
	{
		Entries[Index].Data.SetTileIndex(StartIndex + Index);
		MarkItemDirty(Entries[Index]);
	}
}

int32 FInventoryStorageGridFastArray::GetStackCount(int32 SlotIndex) const
{
	check(Entries.IsValidIndex(SlotIndex));
	return Entries[SlotIndex].Data.GetStackCount();
}

void FInventoryStorageGridFastArray::AddStackCount(int32 SlotIndex, int32 AmountToAdd)
{
	check(Entries.IsValidIndex(SlotIndex));
	SetStackCount(SlotIndex, Entries[SlotIndex].Data.GetStackCount() + AmountToAdd);
}

void FInventoryStorageGridFastArray::SetStackCount(int32 SlotIndex, int32 NewStackCount)
{
	check(Entries.IsValidIndex(SlotIndex));
	FInventoryStorageGridSlot& Slot = Entries[SlotIndex].Data;
	Slot.SetStackCount(NewStackCount);
	MarkItemDirty(Entries[SlotIndex]);
}

void FInventoryStorageGridFastArray::SetInventoryItem(int32 SlotIndex, UInventoryItem* NewItem, int32 NewStartIndex)
{
	check(Entries.IsValidIndex(SlotIndex));
	FInventoryStorageGridSlot& Slot = Entries[SlotIndex].Data;
	Slot.SetInventoryItem(NewItem);
	if (NewStartIndex != INDEX_NONE)
	{
		Slot.SetStartIndex(NewStartIndex);
	}
	MarkItemDirty(Entries[SlotIndex]);
}

void FInventoryStorageGridFastArray::ClearSlot(int32 SlotIndex)
{
	check(Entries.IsValidIndex(SlotIndex));
	FInventoryStorageGridSlot& Slot = Entries[SlotIndex].Data;
	Slot.Clear();
	MarkItemDirty(Entries[SlotIndex]);
}

void FInventoryStorageGridFastArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	LOG_NETFUNCTIONCALL_STRUCT_MSG(OwningGrid, GridSlots, TEXT(" (num=%d)"), AddedIndices.Num())

	if (IsValid(OwningGrid))
	{
		OwningGrid->NotifyGridChanged(AddedIndices);
	}
}

void FInventoryStorageGridFastArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	LOG_NETFUNCTIONCALL_STRUCT_MSG(OwningGrid, GridSlots, TEXT(" (num=%d)"), ChangedIndices.Num())

	if (IsValid(OwningGrid))
	{
		OwningGrid->NotifyGridChanged(ChangedIndices);
	}
}
