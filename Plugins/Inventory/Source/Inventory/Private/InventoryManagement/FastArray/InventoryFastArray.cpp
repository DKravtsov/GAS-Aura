// Copyright 4sandwiches


#include "InventoryManagement/FastArray/InventoryFastArray.h"

#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"


FInventoryEntry::FInventoryEntry()
{
}

TArray<UInventoryItem*> FInventoryFastArray::GetAllItems() const
{
	TArray<UInventoryItem*> Result;
	Result.Reserve(Entries.Num());
	for (const auto& Entry : Entries)
	{
		if (IsValid(Entry.Item))
		{
			Result.Add(Entry.Item);
		}
	}
	return Result;
}

void FInventoryFastArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(OwnerComponent);
	if (IsValid(InventoryComponent))
	{
		for (const int32 Index : RemovedIndices)
		{
			InventoryComponent->OnItemRemoved.Broadcast(Entries[Index].Item);
		}
	}
}

void FInventoryFastArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(OwnerComponent);
	if (IsValid(InventoryComponent))
	{
		for (const int32 Index : AddedIndices)
		{
			InventoryComponent->OnItemAdded.Broadcast(Entries[Index].Item);
		}
	}
}

UInventoryItem* FInventoryFastArray::AddItem(UInventoryItemComponent* ItemComponent)
{
	check(OwnerComponent != nullptr);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	if (UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(OwnerComponent))
	{
		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);
		InventoryComponent->AddRepSubObj(NewEntry.Item);
		MarkItemDirty(NewEntry);
		return NewEntry.Item;
	}
	return nullptr;
}

UInventoryItem* FInventoryFastArray::AddItem(UInventoryItem* Item)
{
	check(OwnerComponent != nullptr);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = Item;
	MarkItemDirty(NewEntry);
	return Item;
}

void FInventoryFastArray::RemoveItem(UInventoryItem* Item)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		const auto& Entry = *EntryIt;
		if (Entry.Item == Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}
