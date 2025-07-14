// Copyright 4sandwiches


#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"

#include "InventoryGlobalSettings.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"
#include "Items/InventoryItem.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "Net/UnrealNetwork.h"

#include "DebugHelper.h"

AActor* UInventorySpatialStorage::GetOwningActor() const
{
	const UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOuter());
	check(InventoryComponent);
	AActor* OwningActor = InventoryComponent->GetOwner();
	check(OwningActor);
	return OwningActor;
}

void UInventorySpatialStorage::SetupStorage()
{
	AActor* OwningActor = GetOwningActor();
	LOG_NETFUNCTIONCALL_OWNER(OwningActor)

	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOuter());
	check(InventoryComponent);
	
	UClass* GridClass = GetStorageGridClass();
	check(GridClass != nullptr);
	
	for (const auto& ItemCategory : GridCategories)
	{
		auto NewGrid = NewObject<UInventoryStorageGrid>(OwningActor, GridClass);
		check(IsValid(NewGrid));
		NewGrid->SetItemCategory(ItemCategory);
		NewGrid->ConstructGrid(Rows, Columns);
        
		InventoryGrids.Emplace(ItemCategory, NewGrid);

		InventoryComponent->AddRepSubObj(NewGrid);
	}
}

TSubclassOf<UInventoryStorageGrid> UInventorySpatialStorage::GetStorageGridClass()
{
	return UInventoryGlobalSettings::GetSpatialStorageGridClass();
}

UInventoryStorageGrid* UInventorySpatialStorage::FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const
{
	if (const auto* GridPtr = InventoryGrids.Find(ItemCategory))
	{
		return GridPtr->Get();
	}
	return nullptr;
}

int32 UInventorySpatialStorage::GetItemIndex(UInventoryItem* Item)
{
	if (IsValid(Item))
	{
		auto Grid = FindInventoryGridByCategory(Item->GetItemManifest().GetItemCategory());
		check(Grid);
		return Grid->GetItemIndex(Item);
	}

	return INDEX_NONE;
}

void UInventorySpatialStorage::UpdateGridSlots(UInventoryItem* NewItem, int32 Index, bool bStackable, int32 StackAmount)
{
	if (IsValid(NewItem))
	{
		auto Grid = FindInventoryGridByCategory(NewItem->GetItemManifest().GetItemCategory());
		check(Grid);
		Grid->UpdateGridSlots(NewItem, Index, bStackable, StackAmount);
	}
}

void UInventorySpatialStorage::RemoveItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex)
{
	if (IsValid(ItemToRemove))
	{
		auto Grid = FindInventoryGridByCategory(ItemToRemove->GetItemManifest().GetItemCategory());
		check(Grid);
		Grid->RemoveItemFromGrid(ItemToRemove, GridIndex);
	}
}

void UInventorySpatialStorage::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventorySpatialStorage, InventoryGrids);
}

FInventorySlotAvailabilityResult UInventorySpatialStorage::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	if (auto* Grid = FindInventoryGridByCategory(ItemManifest.GetItemCategory()))
		return Grid->HasRoomForItem(ItemManifest, StackCountOverride);
	
	UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid Item Category"));
	return FInventorySlotAvailabilityResult{};
}
