// Copyright 4sandwiches


#include "Items/Components/InventoryItemComponent.h"

#include "Items/InventoryItemData.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Net/UnrealNetwork.h"


UInventoryItemComponent::UInventoryItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UInventoryItemComponent::PickedUp()
{
	checkf(GetOwner()->HasAuthority(), TEXT("PickedUp() called on non-authoritative item"));
	
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

#if WITH_EDITOR
void UInventoryItemComponent::CopyManifestFromData()
{
	if (!ItemData.IsNull())
	{
		if (const auto* Data = ItemData.LoadSynchronous())
		{
			ItemManifest = Data->GetItemManifest();
			bOverrideItemManifest = true;
		}
	}
}
#endif//WITH_EDITOR

void UInventoryItemComponent::BeginPlay()
{
	if (GetOwner()->HasAuthority())
	{
		if (!bOverrideItemManifest && !ItemData.IsNull())
		{
			ItemManifest = ItemData.LoadSynchronous()->GetItemManifest();
			if (StackCount > 0)
			{
				if (const auto StackableFragment = GetItemManifestMutable().GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
				{
					StackableFragment->SetStackCount(StackCount);
				}
				StackCount = -1; // reset to defaults
			}
		}
	
		if (!ItemManifest.PickupActorClass)
		{
			ItemManifest.PickupActorClass = GetOwner()->GetClass();
		}
	}
	Super::BeginPlay();
}


