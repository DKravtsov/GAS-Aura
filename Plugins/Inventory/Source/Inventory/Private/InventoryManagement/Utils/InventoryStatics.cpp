// Copyright 4sandwiches


#include "InventoryManagement/Utils/InventoryStatics.h"

#include "InventoryGlobalSettings.h"
#include "EquipmentManagement/Components/InventoryEquipmentComponent.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Store/Components/InventoryStoreComponent.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"

UInventoryComponent* UInventoryStatics::GetInventoryComponent(const AActor* Actor)
{
	if (IsValid(Actor))
	{
		UInventoryComponent* InventoryComponent = Actor->FindComponentByClass<UInventoryComponent>();
		return InventoryComponent;
	}
	return nullptr;
}

UInventoryEquipmentComponent* UInventoryStatics::GetEquipmentComponent(const APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		UInventoryEquipmentComponent* EquipmentComponent = PlayerController->FindComponentByClass<UInventoryEquipmentComponent>();
		return EquipmentComponent;
	}
	return nullptr;
}

UInventoryItemComponent* UInventoryStatics::GetInventoryItemComponent(const AActor* Actor)
{
	if (IsValid(Actor))
	{
		return Actor->FindComponentByClass<UInventoryItemComponent>();
	}
	return nullptr;
}

UInventoryStoreComponent* UInventoryStatics::GetStoreComponent(const AActor* Actor)
{
	if (IsValid(Actor))
	{
		return Actor->FindComponentByClass<UInventoryStoreComponent>();
	}
	return nullptr;
}

FGameplayTag UInventoryStatics::GetItemCategory(UInventoryItemComponent* ItemComponent)
{
	if (!IsValid(ItemComponent))
		return FGameplayTag();

	return ItemComponent->GetItemManifest().GetItemCategory();
}

UInventoryWidgetBase* UInventoryStatics::GetInventoryWidget(const APlayerController* PlayerController)
{
	UInventoryWidgetBase* InventoryMenu = nullptr;
	if (const UInventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController))
	{
		if (InventoryComponent->IsMenuOpen())
		{
			InventoryMenu = InventoryComponent->GetInventoryMenu();
		}
		else if (InventoryComponent->IsStoreMenuOpen())
		{
			InventoryMenu = InventoryComponent->GetStoreMenu();
		}
	}
	return InventoryMenu;
}

void UInventoryStatics::ItemHovered(APlayerController* PlayerController, UInventoryItem* Item)
{
	if (UInventoryWidgetBase* InventoryMenu =GetInventoryWidget(PlayerController))
	{
		if (!InventoryMenu->HasHoverItem())
		{
			InventoryMenu->OnInventoryHovered(Item);
		}
	}
}

void UInventoryStatics::ItemUnhovered(APlayerController* PlayerController)
{
	if (UInventoryWidgetBase* InventoryMenu = GetInventoryWidget(PlayerController))
	{
		InventoryMenu->OnInventoryUnhovered();
	}
}

class UInventoryHoverItemWidget* UInventoryStatics::GetHoverItem(const APlayerController* PlayerController)
{
	const UInventoryWidgetBase* InventoryMenu = GetInventoryWidget(PlayerController);
	return InventoryMenu ? InventoryMenu->GetHoverItem() : nullptr;
}

bool UInventoryStatics::CanEquipItem(const UInventoryItem* Item, const FGameplayTag& EquipmentTypeTag)
{
	if (IsValid(Item) && !Item->IsStackable() && Item->IsEquipable())
	{
		if (const FInventoryItemEquipmentFragment* EquipFragment = Item->GetItemManifest().GetFragmentOfType<FInventoryItemEquipmentFragment>())
		{
			return EquipFragment->GetEquipmentType().MatchesTag(EquipmentTypeTag);
		}
	}
	return false;
}

bool UInventoryStatics::CanEquipItem(const FInventoryItemManifest& ItemManifest, const FGameplayTag& EquipmentTypeTag)
{
	if (!ItemManifest.GetItemCategory().MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment))
		return false;
	
	if (ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>())
		return false;
	
	if (const auto* EquipFragment = ItemManifest.GetFragmentOfType<FInventoryItemEquipmentFragment>())
	{
		return EquipFragment->GetEquipmentType().MatchesTag(EquipmentTypeTag);
	}
	return false;
}

bool UInventoryStatics::IsItemEquipable(const UInventoryItem* Item)
{
	return IsValid(Item) && !Item->IsStackable() && Item->IsEquipable() && Item->GetItemManifest().GetFragmentOfType<FInventoryItemEquipmentFragment>();
}

FGameplayTag UInventoryStatics::GetItemEquipmentTag(const UInventoryItem* Item)
{
	if (IsValid(Item))
	{
		if (const FInventoryItemEquipmentFragment* EquipFragment = Item->GetItemManifest().GetFragmentOfType<FInventoryItemEquipmentFragment>())
		{
			return EquipFragment->GetEquipmentType();
		}
	}
	return FGameplayTag::EmptyTag;
}

const FInventoryItemManifest& UInventoryStatics::GetCoinItemManifest(const UObject* WorldContextObject)
{
	const auto* CoinsItemManifest = UInventoryGlobalSettings::GetCoinsItemManifest();
	checkf(CoinsItemManifest, TEXT("Coins item manifest is not set"));
	return *CoinsItemManifest;
}

int32 UInventoryStatics::GetItemSellValue(const UInventoryItem* Item)
{
	if (IsValid(Item))
	{
		if (const auto* CostFragment = Item->GetItemManifest().GetFragmentOfTypeWithTag<FInventoryItemLabeledValueFragment>(InventoryFragmentTags::FragmentTag_SellValue))
		{
			return FMath::FloorToInt32(CostFragment->GetValue());
		}
	}
	return 0;
}

