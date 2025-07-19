// Copyright 4sandwiches


#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorage.h"

#include "InventoryGlobalSettings.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"
#include "Items/InventoryItem.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "Net/UnrealNetwork.h"

#include "DebugHelper.h"

FInventorySpatialStorageSetupData::FInventorySpatialStorageSetupData()
{
	StorageClass = UInventorySpatialStorage::StaticClass();
	GridCategories.Add(InventoryTags::Inventory_ItemCategory_Equipment); // add default category
}

UInventorySpatialStorage::UInventorySpatialStorage()
{
}

AActor* UInventorySpatialStorage::GetOwningActor() const
{
	const UInventoryComponent* InventoryComponent = GetOwningInventoryComponent();
	check(InventoryComponent);
	AActor* OwningActor = InventoryComponent->GetOwner();
	check(OwningActor);
	return OwningActor;
}

UInventoryComponent* UInventorySpatialStorage::GetOwningInventoryComponent() const
{
	return Cast<UInventoryComponent>(GetOuter());
}

void UInventorySpatialStorage::SetupStorage(const FInventoryStorageSetupData* SetupData)
{
	LOG_NETFUNCTIONCALL

	const FInventorySpatialStorageSetupData& SpatialSetupData = *static_cast<const FInventorySpatialStorageSetupData*>(SetupData);

	UInventoryComponent* InventoryComponent = GetOwningInventoryComponent();
	check(InventoryComponent);

	const UClass* GridClass = GetStorageGridClass();
	check(GridClass != nullptr);

	if (!ensure(SpatialSetupData.GridCategories.Num() > 0))
	{
		UE_LOG(LogInventory, Error, TEXT("Forgot to create grid categories."))
		return;
	}
	
	for (const auto& ItemCategory : SpatialSetupData.GridCategories)
	{
		auto NewGrid = NewObject<UInventoryStorageGrid>(this, GridClass);
		check(IsValid(NewGrid));
		NewGrid->SetItemCategory(ItemCategory);
		NewGrid->ConstructGrid(SpatialSetupData.Rows, SpatialSetupData.Columns);
        
		InventoryGrids.Emplace(NewGrid);

		InventoryComponent->AddRepSubObj(NewGrid);
	}
}

TSubclassOf<UInventoryStorageGrid> UInventorySpatialStorage::GetStorageGridClass()
{
	return UInventoryGlobalSettings::GetSpatialStorageGridClass();
}

UInventoryStorageGrid* UInventorySpatialStorage::FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const
{
	const auto* GridPtr = InventoryGrids.FindByPredicate([ItemCategory](const UInventoryStorageGrid* Grid)
	{
		return Grid->GetItemCategory().MatchesTagExact(ItemCategory);
	});
	return GridPtr ? GridPtr->Get() : nullptr;
}

int32 UInventorySpatialStorage::GetItemIndex(UInventoryItem* Item)
{
	if (IsValid(Item))
	{
		const auto Grid = FindInventoryGridByCategory(Item->GetItemManifest().GetItemCategory());
		check(Grid);
		return Grid->GetItemIndex(Item);
	}

	return INDEX_NONE;
}

int32 UInventorySpatialStorage::GetItemStackCount(UInventoryItem* Item, int32 GridIndex)
{
	if (IsValid(Item))
	{
		const auto Grid = FindInventoryGridByCategory(Item->GetItemManifest().GetItemCategory());
		check(Grid);
		return Grid->GetStackCount(GridIndex);
	}
	return 0;
}

void UInventorySpatialStorage::UpdateGridSlots(UInventoryItem* NewItem, int32 Index, bool bStackable, int32 StackAmount)
{
	if (IsValid(NewItem))
	{
		const auto Grid = FindInventoryGridByCategory(NewItem->GetItemManifest().GetItemCategory());
		check(Grid);
		Grid->UpdateGridSlots(NewItem, Index, bStackable, StackAmount);
	}
}

void UInventorySpatialStorage::RemoveItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex)
{
	if (IsValid(ItemToRemove))
	{
		const auto Grid = FindInventoryGridByCategory(ItemToRemove->GetItemManifest().GetItemCategory());
		check(Grid);
		Grid->RemoveItemFromGrid(ItemToRemove, GridIndex);
	}
}

int32 UInventorySpatialStorage::FillInStacksOrConsumeHover(UInventoryItem* Item, int32 TargetIndex, int32 AddStackCount)
{
	const auto Grid = FindInventoryGridByCategory(Item->GetItemManifest().GetItemCategory());
	check(Grid);
	return Grid->FillInStacksOrConsumeHover(Item, TargetIndex, AddStackCount);
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

void UInventorySpatialStorage::OnRep_InventoryGrids()
{
	LOG_NETFUNCTIONCALL_MSG(TEXT(" (Inventory Grids Num = %d (%s)"), InventoryGrids.Num(), *GetInventoryGridNamesDebugString())

	if (InventoryGrids.Num() > 0 && InventoryGrids[0] == nullptr)
		return;
	
	if (UInventoryComponent* InventoryComponent = GetOwningInventoryComponent())
	{
		InventoryComponent->ReceivedStorageIsReady();
	}
}

void UInventorySpatialStorage::DebugPrintStorage(FStringBuilderBase& Output, TFunctionRef<TCHAR (const UInventoryItem*)> ItemIndexFunc) const
{
	LOG_NETFUNCTIONCALL

	for (const auto& Grid : InventoryGrids)
	{
		int32 ColIndex = 0;
		Output.Appendf(TEXT("\nGrid [%s]:\n"), *Grid->GetItemCategory().ToString());
		const auto AllGridSlots =  Grid->GetGridSlotsCopy();
		for (const auto& GridSlot : AllGridSlots)
		{
			if (const UInventoryItem* Item = GridSlot.GetInventoryItem().Get())
			{
				const FStringBuilderBase::ElementType ItemStr = ItemIndexFunc(Item);
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
			if (ColIndex >= Grid->GetColumns())
			{
				ColIndex = 0;
				Output.AppendChar('\n');
			}
		}
	}
}

FString UInventorySpatialStorage::GetInventoryGridNamesDebugString() const
{
	TArray<FString> GridNames;
	GridNames.Reserve(InventoryGrids.Num());
	for (const auto& Grid : InventoryGrids)
	{
		if (Grid != nullptr)
		{
			GridNames.Add(Grid->GetItemCategory().ToString());
		}
		else
		{
			GridNames.Add(TEXT("(NULL)"));
		}
	}
	if (GridNames.IsEmpty())
		return TEXT("Empty");
	FStringBuilderBase Output;
	Output.Join(GridNames, TEXT(","));
	return Output.ToString();
}
