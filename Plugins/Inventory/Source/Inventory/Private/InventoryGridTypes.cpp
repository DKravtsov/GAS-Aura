// Copyright 4sandwiches


#include "InventoryGridTypes.h"


namespace InventoryTags
{
	/**  Item Categories **/

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory, "Inventory.ItemCategory", "Inventory Item Categories root");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory_Equipment, "Inventory.ItemCategory.Equipment", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory_Consumable, "Inventory.ItemCategory.Consumable", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_ItemCategory_Crafting, "Inventory.ItemCategory.Crafting", "");
	
}

FInventorySlotAvailabilityResult::FInventorySlotAvailabilityResult()
{
}
