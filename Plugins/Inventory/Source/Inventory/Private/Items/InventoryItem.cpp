// Copyright 4sandwiches


#include "Items/InventoryItem.h"

#include "Items/Fragments/InventoryItemFragment.h"
#include "Net/UnrealNetwork.h"

void UInventoryItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItem, ItemManifest);
	DOREPLIFETIME(UInventoryItem, TotalStackCount);
}

bool UInventoryItem::IsStackable() const
{
	if (!bCachedIsStackable.IsSet())
	{
		const auto StackableFragment = GetItemManifest().GetFragmentOfType<FInventoryItemStackableFragment>();
		bCachedIsStackable = StackableFragment != nullptr;
	}
	return bCachedIsStackable.GetValue();
}

const FGameplayTag& UInventoryItem::GetItemType() const
{
	return GetItemManifest().GetItemType();
}

void UInventoryItem::SetItemManifest(const FInventoryItemManifest& InItemManifest)
{
	ItemManifest = FInstancedStruct::Make<FInventoryItemManifest>(InItemManifest);
}
