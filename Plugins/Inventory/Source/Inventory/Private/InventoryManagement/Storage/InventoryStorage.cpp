// Copyright 4sandwiches


#include "InventoryManagement/Storage/InventoryStorage.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Items/InventoryItem.h"

bool UInventoryStorage::HasRoomForItem(FInventorySlotAvailabilityResult& Result, const UInventoryItemComponent* ItemComponent) const
{
	Result = HasRoomForItemInternal(ItemComponent->GetItemManifest(), -1);
	return Result.TotalRoomToFill > 0;
}

bool UInventoryStorage::HasRoomForItem(FInventorySlotAvailabilityResult& Result, const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	Result = HasRoomForItemInternal(ItemManifest, StackCountOverride);
	return Result.TotalRoomToFill > 0;
}

bool UInventoryStorage::HasRoomForItemAtIndex(FInventorySlotAvailabilityResult& Result, const struct FInventoryItemManifest& ItemManifest, const int32 Index, const int32 StackCountOverride) const
{
	unimplemented();
	return false;
}

APlayerController* UInventoryStorage::TryLockItem(UInventoryItem* Item, int32 GridIndex, APlayerController* PlayerController)
{
	checkf(PlayerController->HasAuthority(), TEXT("This method must be run only on the server"))
	
	const auto* Found = LockedItems.FindByPredicate([Item, GridIndex](const FLockedItemData& LockedData)
	{
		return LockedData.Item == Item && LockedData.GridIndex == GridIndex;
	});
	if (Found)
	{
		return Found->PlayerController.Get();
	}
	LockedItems.Emplace(FLockedItemData{ Item, GridIndex, PlayerController });
	return PlayerController;
}

bool UInventoryStorage::UnlockItem(UInventoryItem* Item, int32 GridIndex, APlayerController* PlayerController)
{
	checkf(PlayerController->HasAuthority(), TEXT("This method must be run only on the server"))
	const int32 FoundIndex = LockedItems.IndexOfByPredicate([Item, GridIndex](const FLockedItemData& LockedData)
	{
		return LockedData.Item == Item && LockedData.GridIndex == GridIndex;
	});
	if (FoundIndex != INDEX_NONE)
	{
		const auto& FoundData = LockedItems[FoundIndex];
		if (FoundData.PlayerController == PlayerController)
		{
			LockedItems.RemoveAtSwap(FoundIndex, 1, EAllowShrinking::No);
			return true;
		}
	}
	return false;
}

FInventorySlotAvailabilityResult UInventoryStorage::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	return FInventorySlotAvailabilityResult{};
}
