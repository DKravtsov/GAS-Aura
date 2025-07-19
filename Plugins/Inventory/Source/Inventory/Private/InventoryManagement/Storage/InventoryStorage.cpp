// Copyright 4sandwiches


#include "InventoryManagement/Storage/InventoryStorage.h"
#include "Items/Components/InventoryItemComponent.h"

bool UInventoryStorage::HasRoomForItem(FInventorySlotAvailabilityResult& Result, const UInventoryItemComponent* ItemComponent) const
{
	Result = HasRoomForItemInternal(ItemComponent->GetItemManifest(), ItemComponent->GetStackCount() > 0 ? ItemComponent->GetStackCount() : -1);
	return Result.TotalRoomToFill > 0;
}

bool UInventoryStorage::HasRoomForItem(FInventorySlotAvailabilityResult& Result, const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	Result = HasRoomForItemInternal(ItemManifest, StackCountOverride);
	return Result.TotalRoomToFill > 0;
}

FInventorySlotAvailabilityResult UInventoryStorage::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	return FInventorySlotAvailabilityResult{};
}
