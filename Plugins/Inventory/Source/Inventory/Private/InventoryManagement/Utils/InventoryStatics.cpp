// Copyright 4sandwiches


#include "InventoryManagement/Utils/InventoryStatics.h"

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
	if (UInventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController))
	{
		return InventoryComponent->GetInventoryMenu();
	}
	return nullptr;
}

void UInventoryStatics::ItemHovered(APlayerController* PlayerController, UInventoryItem* Item)
{
	if (UInventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController))
	{
		if (UInventoryWidgetBase* InventoryMenu = InventoryComponent->GetInventoryMenu())
		{
			if (!InventoryMenu->HasHoverItem())
			{
				InventoryMenu->OnInventoryHovered(Item);
			}
		}
	}
}

void UInventoryStatics::ItemUnhovered(APlayerController* PlayerController)
{
	if (const UInventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController))
	{
		if (UInventoryWidgetBase* InventoryMenu = InventoryComponent->GetInventoryMenu())
		{
			InventoryMenu->OnInventoryUnhovered();
		}
	}
}

class UInventoryHoverItemWidget* UInventoryStatics::GetHoverItem(const APlayerController* PlayerController)
{
	if (const UInventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController))
	{
		if (const UInventoryWidgetBase* InventoryMenu = InventoryComponent->GetInventoryMenu())
		{
			return InventoryMenu->GetHoverItem();
		}
	}
	return nullptr;
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

