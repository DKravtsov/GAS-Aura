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

	auto* InventoryComponent = Cast<UInventoryManagementComponentBase>(OwnerComponent);
	if (IsValid(InventoryComponent))
	{
		for (const int32 Index : RemovedIndices)
		{
			BROADCAST_WITH_LOG_STRUCT(OwnerComponent.Get(), InventoryList, InventoryComponent->OnItemRemoved, Entries[Index].Item);
		}
	}
}

void FInventoryFastArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	LOG_NETFUNCTIONCALL_STRUCT_MSG(OwnerComponent.Get(), InventoryList, TEXT(" (num=%d)"), AddedIndices.Num())

	
	if (const auto* InventoryComponent = Cast<UInventoryManagementComponentBase>(OwnerComponent);IsValid(InventoryComponent))
	{
		for (const int32 Index : AddedIndices)
		{
			BROADCAST_WITH_LOG_STRUCT(OwnerComponent.Get(), InventoryList, InventoryComponent->OnItemAdded, Entries[Index].Item);
		}
	}

	if (auto* InventoryComponent = Cast<UInventoryComponent>(OwnerComponent))
	{
		InventoryComponent->ReceivedStartupItems();
	}
}

UInventoryItem* FInventoryFastArray::AddItem(UInventoryItemComponent* ItemComponent)
{
	check(OwnerComponent != nullptr);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	if (auto* InventoryComponent = Cast<UInventoryManagementComponentBase>(OwnerComponent))
	{
		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Item = ItemComponent->GetItemManifestMutable().Manifest(InventoryComponent);
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

	if (auto* InventoryComponent = Cast<UInventoryManagementComponentBase>(OwnerComponent))
	{
		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Item = ItemManifest.ManifestCopy(InventoryComponent);
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

bool FInventoryFastArray::Contains(UInventoryItem* Item) const
{
	const auto FoundItem = Entries.FindByPredicate([Item](const FInventoryEntry& Entry)
	{
		return Entry.Item == Item;
	});
	return FoundItem && IsValid(FoundItem->Item);
	
}
