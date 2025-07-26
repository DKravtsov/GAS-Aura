// Copyright 4sandwiches


#include "InventoryManagement/Storage/InventoryStorage.h"
#include "Items/Components/InventoryItemComponent.h"

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

FInventorySlotAvailabilityResult UInventoryStorage::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	return FInventorySlotAvailabilityResult{};
}
