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

FText UInventoryItemComponent::GetPickupMessage() const
{
	static const FText FmtNonStackable = NSLOCTEXT("Inventory", "PickupMessageFmt", "Pick up {Name}");
	static const FText FmtStackable = NSLOCTEXT("Inventory", "PickupMessageStackableFmt", "Pick up {Name} ({Count})");
	
	if (bOverridePickupMessage)
	{
		return GetFormattedMessage(PickupMessage);
	}

	return GetFormattedMessage(bCachedStackable ? FmtStackable : FmtNonStackable);
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

	bCachedStackable = nullptr != ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>();
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

	bCachedStackable = nullptr != ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>();
}

FText UInventoryItemComponent::GetFormattedMessage(const FText& Fmt) const
{
	return FText::FormatNamed(Fmt, TEXT("Name"), GetItemDisplayName(), TEXT("Count"), GetStackSize());
}

namespace
{
	const FText& GetEmptyDisplayName()
	{
#if UE_BUILD_SHIPPING || UE_BUILD_TEST
		return FText::GetEmpty();
#else
		static const auto EmptyDisplayName = NSLOCTEXT("Inventory", "InvalidItemName", "(Invalid Name)");
		return EmptyDisplayName;
#endif
	}
}

const FText& UInventoryItemComponent::GetItemDisplayName() const
{
	auto* TextFragment = ItemManifest.GetFragmentOfTypeWithTag<FInventoryItemTextFragment>(InventoryFragmentTags::FragmentTag_ItemName);
	return TextFragment ? TextFragment->GetFragmentText() : GetEmptyDisplayName();
}

int32 UInventoryItemComponent::GetStackSize() const
{
	auto* StackableFragment = ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>();
	return StackableFragment ? StackableFragment->GetStackCount() : 1;
}

