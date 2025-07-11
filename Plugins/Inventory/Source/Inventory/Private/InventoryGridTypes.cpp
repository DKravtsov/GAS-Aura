// Copyright 4sandwiches


#include "InventoryGridTypes.h"
#include "Items/InventoryItem.h"

namespace InventoryTags
{
	/**  Item Categories **/

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory, "Inventory.ItemCategory", "Inventory Item Categories root");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory_Equipment, "Inventory.ItemCategory.Equipment", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory_Consumable, "Inventory.ItemCategory.Consumable", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory_Crafting, "Inventory.ItemCategory.Crafting", "");
	
	/**  Item Categories **/

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_EquipmentSlots, "Inventory.EquipmentSlots", "Slot names for equipment");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_EquipmentSlots_Armour, "Inventory.EquipmentSlots.Armour", "Slot for any armor");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_EquipmentSlots_Weapon, "Inventory.EquipmentSlots.Weapon", "Slot for any weapon");
}

FInventorySlotAvailabilityResult::FInventorySlotAvailabilityResult()
{
}

FInventorySlotAvailabilityResult FInventorySlotAvailabilityResult::Make(UInventoryItem* InItem, int32 InStartIndex)
{
	FInventorySlotAvailabilityResult Result;
	Result.Item = InItem;
	Result.TotalRoomToFill = 1;
	auto& Slot = Result.SlotAvailabilities.AddDefaulted_GetRef();
	Slot.Amount = 1;
	Slot.Index = InStartIndex;
	Slot.bItemAtIndex = true;
	return Result;
}

FInventorySlotAvailabilityResult FInventorySlotAvailabilityResult::Make(UInventoryItem* InItem, int32 InStartIndex, int32 InMaxStackSize, int32 InAmount)
{
	FInventorySlotAvailabilityResult Result;
	Result.Item = InItem;
	Result.TotalRoomToFill = InAmount;
	Result.Remainder = InAmount;
	for (int32 Index = InStartIndex; Result.Remainder > 0; Index++)
	{
		auto& Slot = Result.SlotAvailabilities.AddDefaulted_GetRef();
		Slot.Amount = FMath::Min(InAmount, InMaxStackSize);
		Slot.Index = Index;
		Slot.bItemAtIndex = Slot.Index == InStartIndex;
		Result.Remainder -= Slot.Amount;
	}
	return Result;
}

void FInventoryStorageGridSlot::SetInventoryItem(UInventoryItem* InItem)
{
	InventoryItem = InItem;
}

void FInventoryEquipmentSlot::SetInventoryItem(UInventoryItem* InItem)
{
	InventoryItem = InItem;
}
