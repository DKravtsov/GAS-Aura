// Copyright 4sandwiches


#include "Items/InventoryItem.h"

#include "InventoryGridTypes.h"
#include "InventoryManagement/Components/InventoryManagementComponentBase.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
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

bool UInventoryItem::IsConsumable() const
{
	return GetItemManifest().GetItemCategory().MatchesTagExact(InventoryTags::Inventory_ItemCategory_Consumable);
}

bool UInventoryItem::IsEquipable() const
{
	return GetItemManifest().GetItemCategory().MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment);
}

const FGameplayTag& UInventoryItem::GetItemType() const
{
	return GetItemManifest().GetItemType();
}

UInventoryStorage* UInventoryItem::GetOwningStorage() const
{
	if (!CachedOwningStorage.IsValid())
	{
		const auto* OwningComponent = Cast<UInventoryManagementComponentBase>(GetOuter());
		CachedOwningStorage = OwningComponent->GetInventoryStorage();
	}
	return CachedOwningStorage.Get();
}

void UInventoryItem::SetItemManifest(const FInventoryItemManifest& InItemManifest)
{
	ItemManifest = FInstancedStruct::Make<FInventoryItemManifest>(InItemManifest);
}
