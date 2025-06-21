// Copyright 4sandwiches


#include "Items/Manifest/InventoryItemManifest.h"

#include "Items/InventoryItem.h"


FInventoryItemManifest::FInventoryItemManifest()
{
}

UInventoryItem* FInventoryItemManifest::Manifest(UObject* NewOuter)
{
	auto NewItem = NewObject<UInventoryItem>(NewOuter, UInventoryItem::StaticClass());
	NewItem->SetItemManifest(*this);

	return NewItem;
}
