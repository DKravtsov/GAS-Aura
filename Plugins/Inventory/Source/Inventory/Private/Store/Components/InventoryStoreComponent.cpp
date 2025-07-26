// Copyright 4sandwiches


#include "Store/Components/InventoryStoreComponent.h"

#include "Components/SlateWrapperTypes.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"

#include "DebugHelper.h"
#include "InventoryManagement/Data/InventorySetupData.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"


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

}

void UInventoryStoreComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryStoreComponent, InventoryList);
	DOREPLIFETIME(UInventoryStoreComponent, InventoryStorage);
}

int32 UInventoryStoreComponent::GetSellValue(const UInventoryItem* Item, const int32 StackCount) const
{
	return UInventoryStatics::GetItemSellValue(Item) * StackCount;
}

bool UInventoryStoreComponent::HasCoins(int32 SellValue)
{
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

void UInventoryStoreComponent::TryAddItem(const FInventoryItemManifest& ItemManifest, int32 StackCount)
{
}

