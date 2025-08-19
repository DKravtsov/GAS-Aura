// Copyright 4sandwiches


#include "Store/Components/InventoryLootContainerComponent.h"


int32 UInventoryLootContainerComponent::GetSellValue(const UInventoryItem* Item, int32 StackCount) const
{
	return 0; // free
}

int32 UInventoryLootContainerComponent::GetPurchaseValue(const UInventoryItem* Item, int32 StackCount) const
{
	return 0; // free
}
