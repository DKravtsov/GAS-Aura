// Copyright 4sandwiches


#include "InventoryManagement/Utils/InventoryStatics.h"

#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/Components/InventoryItemComponent.h"

UInventoryComponent* UInventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		UInventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UInventoryComponent>();
		return InventoryComponent;
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
