// Copyright 4sandwiches

#pragma once

#include "DebugHelper.h"

#include "Items/InventoryItem.h"

const TCHAR* GetInventoryItemId(const class UInventoryItem* Item)
{
	return Item ? *Item->GetItemType().ToString() : TEXT("None");
}
