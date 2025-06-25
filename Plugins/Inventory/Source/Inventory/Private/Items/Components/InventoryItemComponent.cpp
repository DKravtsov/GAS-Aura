// Copyright 4sandwiches


#include "Items/Components/InventoryItemComponent.h"

#include "Net/UnrealNetwork.h"


UInventoryItemComponent::UInventoryItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UInventoryItemComponent::PickedUp()
{
	BP_PickedUp();

	GetOwner()->Destroy();
}

void UInventoryItemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemComponent, ItemManifest);
}

void UInventoryItemComponent::InitItemManifestFrom(const FInventoryItemManifest& ItemManifestToCopy)
{
	ItemManifest = ItemManifestToCopy;
}

void UInventoryItemComponent::BeginPlay()
{
	if (!ItemManifest.PickupActorClass)
	{
		ItemManifest.PickupActorClass = GetOwner()->GetClass();
	}
	Super::BeginPlay();
}


