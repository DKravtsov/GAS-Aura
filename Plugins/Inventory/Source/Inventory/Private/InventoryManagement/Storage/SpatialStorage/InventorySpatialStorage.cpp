// Copyright 4sandwiches


#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"

#include "InventoryGlobalSettings.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"
#include "Items/InventoryItem.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "Net/UnrealNetwork.h"

#include "DebugHelper.h"

UInventorySpatialStorage::UInventorySpatialStorage()
{
	GridCategories.Add(InventoryTags::Inventory_ItemCategory_Equipment); // add default category
}

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

	if (!ensure(GridCategories.Num() > 0))
	{
		UE_LOG(LogInventory, Error, TEXT("Forgot to create grid categories."))
		return;
	}
	
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

void UInventorySpatialStorage::DebugPrintStorage() const
{
	const AActor* OwningActor = GetOwningActor();
	LOG_NETFUNCTIONCALL_OWNER(OwningActor)
	
	FStringBuilderBase Output;
	TMap<const UInventoryItem*, FStringBuilderBase::ElementType> ItemIndexMap;
	int32 ItemIndex = 0;
	for (const auto& [Tag, Grid] : InventoryGrids)
	{
		int32 ColIndex = 0;
		Output.Appendf(TEXT("\nGrid [%s]:\n"), *Grid->GetItemCategory().ToString());
		for (const auto& GridSlot : Grid->GetGridSlotsCopy())
		{
			if (const UInventoryItem* Item = GridSlot.GetInventoryItem().Get())
			{
				FStringBuilderBase::ElementType ItemStr;
				if (auto Found = ItemIndexMap.Find(Item))
				{
					ItemStr = *Found;
				}
				else
				{
					ItemStr = 'A' + ItemIndex; 
					ItemIndexMap.Add(Item, ItemStr);
					++ItemIndex;
				}
				
				Output.Appendf(TEXT("[%c"), ItemStr);
				if (Item->IsStackable())
					Output.Appendf(TEXT(",%d"), GridSlot.GetStackCount());
				Output.AppendChar(']');
			}
			else
			{
				Output.Append(TEXT("[.]"));
			}
			ColIndex++;
			if (ColIndex >= Columns)
			{
				ColIndex = 0;
				Output.AppendChar('\n');
			}
		}
	}
	Output.AppendChar('\n');
	for (const auto& [Item, Index] : ItemIndexMap)
	{
		Output.Appendf(TEXT("%c: %s\n"), Index, *Item->GetItemType().ToString());
	}

	UE_LOG(LogInventory, Warning, TEXT("Print Storage:\n%s"), Output.ToString())
}

FString UInventorySpatialStorage::GetInventoryGridNamesDebugString() const
{
	TArray<FString> GridNames;
	GridNames.Reserve(InventoryGrids.Num());
	for (const auto& [Tag, Grid] : InventoryGrids)
	{
		GridNames.Add(Tag.ToString());
	}
	FStringBuilderBase Output;
	Output.Join(GridNames, TEXT(","));
	return Output.ToString();
}
