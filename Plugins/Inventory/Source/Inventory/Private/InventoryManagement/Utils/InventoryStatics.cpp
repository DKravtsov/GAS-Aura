// Copyright 4sandwiches


#include "InventoryManagement/Utils/InventoryStatics.h"

#include "EquipmentManagement/Components/InventoryEquipmentComponent.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"

UInventoryComponent* UInventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		UInventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UInventoryComponent>();
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

class UInventoryHoverProxy* UInventoryStatics::GetHoverItem(const APlayerController* PlayerController)
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
