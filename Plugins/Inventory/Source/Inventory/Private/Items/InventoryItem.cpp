// Copyright 4sandwiches


#include "Items/InventoryItem.h"

#include "Net/UnrealNetwork.h"

void UInventoryItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItem, ItemManifest);
}

void UInventoryItem::SetItemManifest(const FInventoryItemManifest& InItemManifest)
{
	ItemManifest = FInstancedStruct::Make<FInventoryItemManifest>(InItemManifest);
}
