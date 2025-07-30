// Copyright 4sandwiches


#include "Store/Components/InventoryStoreComponent.h"

#include "Engine/AssetManager.h"
#include "InventoryManagement/Data/InventorySetupData.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/InventoryItemData.h"

#include "Net/UnrealNetwork.h"

#include "DebugHelper.h"

// Sets default values for this component's properties
UInventoryStoreComponent::UInventoryStoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

}

void UInventoryStoreComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AddStartupItems();
	}
}

void UInventoryStoreComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryStoreComponent, InventoryList);
	DOREPLIFETIME(UInventoryStoreComponent, InventoryStorage);
	DOREPLIFETIME(UInventoryStoreComponent, bStartupItemsInitialized);
}

int32 UInventoryStoreComponent::GetSellValue(const UInventoryItem* Item, const int32 StackCount) const
{
	return UInventoryStatics::GetItemSellValue(Item) * (Item->IsStackable() ? StackCount : 1);
}

bool UInventoryStoreComponent::HasCoins(int32 SellValue)
{
	if (SellValue <= 0)
		return true;
	
	if (const auto Item = InventoryList.FindFirstItemByType(InventoryTags::GameItems_Collectables_Coins))
	{
		return Item->GetTotalStackCount() >= SellValue;
	}
	return false;
}

void UInventoryStoreComponent::RemoveCoins(int32 SellValue)
{
	checkf(GetOwner()->HasAuthority(), TEXT("RemoveCoins() called on non-authoritative item"));

	
}

void UInventoryStoreComponent::AddStartupItems()
{
	LOG_NETFUNCTIONCALL

	checkf(HasAuthority(), TEXT("AddStartupItems() called on non-authoritative object."));

	const TArray<FInventoryItemProxy>& StartupInventoryItems = InventorySetupData->DefaultStartupInventory;

	TArray<FSoftObjectPath> AssetList;
	AssetList.Reserve(StartupInventoryItems.Num());

	for (const auto& Item : StartupInventoryItems)
	{
		AssetList.Add(Item.InventoryItem.ToSoftObjectPath());
	}
	
	UAssetManager::Get().LoadAssetList(AssetList, FStreamableDelegate::CreateUObject(this, &UInventoryStoreComponent::StartupItemsAssetsLoaded), FStreamableManager::DefaultAsyncLoadPriority, TEXT("Store:LoadAssets"));
	
}

void UInventoryStoreComponent::StartupItemsAssetsLoaded()
{
	LOG_NETFUNCTIONCALL

	const TArray<FInventoryItemProxy>& StartupInventoryItems = InventorySetupData->DefaultStartupInventory;
	for (const auto& Item : StartupInventoryItems)
	{
		// all data should be already loaded at this point
		if (const auto* ItemData = Item.InventoryItem.LoadSynchronous())
		{
			int32 StackCount = -1;
			if (const auto* StackableFragment = ItemData->GetItemManifest().GetFragmentOfType<FInventoryItemStackableFragment>())
			{
				StackCount = Item.bOverrideCount
					? FMath::RandRange(Item.MinMaxAmount.X, Item.MinMaxAmount.Y)
					: StackableFragment->GetStackCount();
			}

			TryAddItem(ItemData->GetItemManifest(), StackCount);
		}
	}
	bStartupItemsInitialized = true;
}

void UInventoryStoreComponent::OnRep_StartupItemsInitialized()
{
	LOG_NETFUNCTIONCALL
}

//#if UE_WITH_CHEAT_MANAGER
void UInventoryStoreComponent::DebugPrintStorage() const
{
	if (!IsValid(InventoryStorage))
	{
		DebugPrint(TEXT("DebugPrintStore: Inventory Storage is null"), FColor::Red);
		return;
	}

	TArray<UInventoryItem*> AllItems = InventoryList.GetAllItems();

	// For now, each item will have its own letter A-Z. It can be changed in future
	auto GetItemIndex = [&AllItems](const UInventoryItem* Item) -> TCHAR
	{
		return Item ? 'A' + AllItems.IndexOfByKey(Item) : '-';
	};
	checkf(AllItems.Num() < ('Z' - 'A'), TEXT("Probably we need more robust identification here"));
	FStringBuilderBase Output;
	Output.Appendf(TEXT("Store Debug Print [%s]\n"), *GetOwner()->GetName());
	Output.Append(TEXT("Items:\n"));
	for (int32 Index = 0; Index < AllItems.Num(); Index++)
	{
		Output.Appendf(TEXT("%c: %s\n"), 'A' + Index, *GetInventoryItemId(AllItems[Index]));
	}
	Output.AppendChar('\n');

	Output.Append(TEXT("Storage:\n"));
	InventoryStorage->DebugPrintStorage(Output, GetItemIndex);

	UE_LOG(LogTemp, Warning, TEXT("%s"), Output.ToString());
}
//#endif//UE_WITH_CHEAT_MANAGER
