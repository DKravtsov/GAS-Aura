// Copyright 4sandwiches


#include "InventoryManagement/Components/InventoryManagementComponentBase.h"
#include "InventoryManagement/Data/InventorySetupData.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "Net/UnrealNetwork.h"

#include "DebugHelper.h"
#include "Items/InventoryItem.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Items/Manifest/InventoryItemManifest.h"


UInventoryManagementComponentBase::UInventoryManagementComponentBase()
	: InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventoryManagementComponentBase::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
}

void UInventoryManagementComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryManagementComponentBase, InventoryList);
	DOREPLIFETIME(UInventoryManagementComponentBase, InventoryStorage);
}

void UInventoryManagementComponentBase::BeginPlay()
{
	LOG_NETFUNCTIONCALL

	if (GetOwner()->HasAuthority())
	{
		CreateInventoryStorage();
	}

	Super::BeginPlay();
}

void UInventoryManagementComponentBase::CreateInventoryStorage()
{
	LOG_NETFUNCTIONCALL

	check(GetOwner()->HasAuthority());
	
	if (!ensure(!IsValid(InventoryStorage)))
		return;

	check(!InventorySetupData.IsNull());

	const FInventoryStorageSetupData* SetupData = InventorySetupData.LoadSynchronous()->InventoryStorage.GetData();
	check(SetupData != nullptr);

	InventoryStorage = NewObject<UInventoryStorage>(this, SetupData->StorageClass);	
	check(InventoryStorage);
	AddRepSubObj(InventoryStorage);

	InventoryStorage->SetupStorage(SetupData);
}

UInventoryItem* UInventoryManagementComponentBase::TryAddItem(const FInventoryItemManifest& ItemManifest, int32 StackCount)
{
	LOG_NETFUNCTIONCALL
	
	checkf(HasAuthority(), TEXT("This method should run only on server"));

	FInventorySlotAvailabilityResult Result;
	if (!InventoryStorage->HasRoomForItem(Result, ItemManifest, StackCount))
	{
		UE_LOG(LogInventory, Error, TEXT("Adding startup item [%s]: inventory has no room for %d items."),
			*ItemManifest.GetItemType().ToString(), StackCount);
		return nullptr;
	}
	UInventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemManifest.GetItemType());
	Result.Item = FoundItem;

	if (Result.Remainder > 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("Adding startup item [%s]: inventory has not enough room for %d more item(s) and they will be destroyed."),
		   *ItemManifest.GetItemType().ToString(), Result.Remainder);
	}

	if (Result.Item.IsValid() && Result.bStackable)
	{
		BROADCAST_WITH_LOG(OnStackChanged, Result);
		// Add stacks to an item that already exists in the inventory.Only need to update the stack count
		AddStacksToItem(ItemManifest, Result.TotalRoomToFill);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		return AddNewItem(ItemManifest, Result.bStackable ? Result.TotalRoomToFill : 1);
	}
	return nullptr;
}

bool UInventoryManagementComponentBase::RemoveItemFromInventory(UInventoryItem* Item, int32 StackCount)
{
	checkf(GetOwner()->HasAuthority(), TEXT("This method must be run only on server."))
	
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s], stack count [%d]"), *GetInventoryItemId(Item), StackCount)

	if (!IsValid(Item) || (Item->IsStackable() && StackCount <= 0))
		return false;

	checkf(Item->GetOwningStorage() == InventoryStorage, TEXT("This should process items only from its own storage"))

	StackCount = FMath::Clamp(StackCount, 1, Item->GetTotalStackCount());
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;

	if (NewStackCount <= 0)
	{
		InventoryList.RemoveItem(Item);

		BROADCAST_WITH_LOG(OnItemRemoved, Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}
	return true;
}

bool UInventoryManagementComponentBase::HasEnoughCoins(int32 Price) const
{
	if (Price <= 0)
		return true;
	
	if (const auto Item = InventoryList.FindFirstItemByType(InventoryTags::GameItems_Collectables_Coins))
	{
		return Item->GetTotalStackCount() >= Price;
	}
	return false;
}

void UInventoryManagementComponentBase::RemoveCoins(int32 SellValue)
{
	checkf(GetOwner()->HasAuthority(), TEXT("RemoveCoins() called on non-authoritative item"));

	if (const auto Item = InventoryList.FindFirstItemByType(InventoryTags::GameItems_Collectables_Coins))
	{
		if (SellValue > Item->GetTotalStackCount())
		{
			UE_LOG(LogInventory, Error, TEXT("Trying to remove more coins than has: %d, when has only %d"), SellValue, Item->GetTotalStackCount());
			return;
		}
		FInventorySlotAvailabilityResult Result;
		if (ensure(InventoryStorage->FindItemStacks(Result, Item, SellValue)))
		{
			for (const auto& AvailabilitySlot : Result.SlotAvailabilities)
			{
				RemoveItemFromInventory(Item, AvailabilitySlot.Amount);
				InventoryStorage->RemoveItemFromGrid(Item, AvailabilitySlot.Index, AvailabilitySlot.Amount);
			}
		}
	}
}

bool UInventoryManagementComponentBase::IsValidItem(UInventoryItem* Item, int32 GridIndex, int32 StackCount) const
{
	if (!IsValid(Item))
		return false;

	if (!InventoryList.Contains(Item))
		return false;

	if (Item->IsStackable())
	{
		if (GridIndex != INDEX_NONE)
		{
			if (StackCount != InventoryStorage->GetItemStackCount(Item, GridIndex))
				return false;
		}
		if (StackCount > Item->GetTotalStackCount())
			return false;
	}

	return true;
}

void UInventoryManagementComponentBase::AddItemAtIndex(UInventoryItem* Item, int32 Index, bool bStackable, int32 StackCount)
{
	checkf(IsValid(Item) && Item->GetOwningStorage() == InventoryStorage, TEXT("Item [%s] is not in this inventory"), *GetInventoryItemId(Item))
	FInventorySlotAvailabilityResult Result;
	InventoryStorage->HasRoomForItemAtIndex(Result, Item->GetItemManifest(), Index, StackCount);
	Result.Item = Item;
	BROADCAST_WITH_LOG(OnStackChanged, Result);
}

UInventoryItem* UInventoryManagementComponentBase::AddNewItem(const FInventoryItemManifest& ItemManifest, int32 StackCount)
{
	LOG_NETFUNCTIONCALL
	
	check(GetOwner()->HasAuthority());

	const auto NewItem = InventoryList.AddItem(ItemManifest, StackCount);
	check(NewItem != nullptr);
	NewItem->SetTotalStackCount(StackCount);
	if (const auto StackableFragment = NewItem->GetItemManifestMutable().GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}

	BROADCAST_WITH_LOG(OnItemAdded, NewItem);

	return NewItem;
}

void UInventoryManagementComponentBase::AddStacksToItem(const FInventoryItemManifest& ItemManifest, int32 StackCount)
{
	LOG_NETFUNCTIONCALL
	
	checkf(GetOwner()->HasAuthority(), TEXT("UInventoryComponent::AddStacksToItem called on non-authoritative object."));
	
	const FGameplayTag& ItemType = ItemManifest.GetItemType();
	if (UInventoryItem* Item = InventoryList.FindFirstItemByType(ItemType))
	{
		Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);
	}
}

bool UInventoryManagementComponentBase::FindItemStacks(UInventoryItem* Item, int32 TotalCount, FInventorySlotAvailabilityResult& Result) const
{
	checkf(IsValid(Item) && Item->GetOwningStorage() == InventoryStorage, TEXT("This should process items only from its own storage"))
	return InventoryStorage->FindItemStacks(Result, Item, TotalCount);
}

void UInventoryManagementComponentBase::RemoveItemFromStorage(UInventoryItem* Item, int32 GridIndex)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Removing item [%s] from storage index %d"), *GetInventoryItemId(Item), GridIndex)
	checkf(GetOwner()->HasAuthority(), TEXT("This method should be run only on server"));

	if (!IsValid(Item))
		return;

	checkf(Item->GetOwningStorage() == InventoryStorage, TEXT("This should process items only from its own storage"))
	
	check(InventoryStorage);
	InventoryStorage->RemoveItemFromGrid(Item, GridIndex);
}
