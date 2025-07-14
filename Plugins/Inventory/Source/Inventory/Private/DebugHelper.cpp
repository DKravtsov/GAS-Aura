// Copyright 4sandwiches

#pragma once

#include "DebugHelper.h"

#include "Items/InventoryItem.h"

FString GetInventoryItemId(const class UInventoryItem* Item)
{
	return Item ? Item->GetItemType().ToString() : FString(TEXT("None"));
}
