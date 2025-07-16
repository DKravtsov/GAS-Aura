// Copyright 4sandwiches


#include "InventoryManagement/FastArray/InventoryFastArray.h"

#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Items/Fragments/InventoryItemFragment.h"

#include "DebugHelper.h"


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
	LOG_NETFUNCTIONCALL_STRUCT_MSG(OwnerComponent.Get(), InventoryList, TEXT(" (num=%d)"), RemovedIndices.Num())

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
	LOG_NETFUNCTIONCALL_STRUCT_MSG(OwnerComponent.Get(), InventoryList, TEXT(" (num=%d)"), AddedIndices.Num())

	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(OwnerComponent);
	if (IsValid(InventoryComponent))
	{
		for (const int32 Index : AddedIndices)
		{
			InventoryComponent->OnItemAdded.Broadcast(Entries[Index].Item);
		}
	}

	InventoryComponent->ReceivedStartupItems();
}

UInventoryItem* FInventoryFastArray::AddItem(UInventoryItemComponent* ItemComponent)
{
	check(OwnerComponent != nullptr);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	if (UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(OwnerComponent))
	{
		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Item = ItemComponent->GetItemManifestMutable().Manifest(OwningActor);
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

UInventoryItem* FInventoryFastArray::AddItem(const FInventoryItemManifest& ItemManifest, const int32 OverrideCount)
{
	check(OwnerComponent != nullptr);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	if (UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(OwnerComponent))
	{
		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Item = ItemManifest.ManifestCopy(OwningActor);
		if (OverrideCount > 0)
		{
			FInventoryItemManifest& NewManifest = NewEntry.Item->GetItemManifestMutable();
			if (auto* StackableFragment = NewManifest.GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
			{
				StackableFragment->SetStackCount(OverrideCount);
			}
		}
		InventoryComponent->AddRepSubObj(NewEntry.Item);
		MarkItemDirty(NewEntry);
		return NewEntry.Item;
	}
	return nullptr;
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

UInventoryItem* FInventoryFastArray::FindFirstItemByType(const FGameplayTag& ItemType) const
{
	const auto FoundItem = Entries.FindByPredicate([ItemType](const FInventoryEntry& Entry)
	{
		return IsValid(Entry.Item) && Entry.Item->GetItemType().MatchesTagExact(ItemType);
	});
	return FoundItem ? ToRawPtr(FoundItem->Item) : nullptr;
}
