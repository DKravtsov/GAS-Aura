// Copyright 4sandwiches


#include "EquipmentManagement/Components/InventoryEquipmentComponent.h"

#include "EquipmentManagement/EquipActor/InventoryEquipActor.h"
#include "GameFramework/Character.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/InventoryItem.h"
#include "Items/Fragments/InventoryItemFragment.h"


UInventoryEquipmentComponent::UInventoryEquipmentComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	
}

void UInventoryEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwningPlayerController = Cast<APlayerController>(GetOwner()); OwningPlayerController.IsValid())
	{
		if (ACharacter* OwnerCharacter = OwningPlayerController->GetCharacter())
		{
			OwningSkeletalMesh = OwnerCharacter->GetMesh();
		}
		InitInventoryComponent();
	}
}

void UInventoryEquipmentComponent::InitInventoryComponent()
{
	InventoryComponent = OwningPlayerController->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent.IsValid())
		return;

	if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &UInventoryEquipmentComponent::OnItemEquipped))
	{
		InventoryComponent->OnItemEquipped.AddDynamic(this, &UInventoryEquipmentComponent::OnItemEquipped);
	}
	if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &UInventoryEquipmentComponent::OnItemUnequipped))
	{
		InventoryComponent->OnItemUnequipped.AddDynamic(this, &UInventoryEquipmentComponent::OnItemUnequipped);
	}
}

AInventoryEquipActor* UInventoryEquipmentComponent::SpawnEquippedActor(
	FInventoryItemEquipmentFragment& EquipmentFragment,
	const FInventoryItemManifest& ItemManifest,
	USkeletalMeshComponent* ParentMesh)
{
	AInventoryEquipActor* SpawnedActor = EquipmentFragment.SpawnEquippedActor(ParentMesh);
	if (SpawnedActor)
	{
		SpawnedActor->SetOwner(GetOwner());
		SpawnedActor->SetEquipmentType(EquipmentFragment.GetEquipmentType());
		EquipmentFragment.SetEquippedActor(SpawnedActor);
	}
	return SpawnedActor;
}

void UInventoryEquipmentComponent::OnItemEquipped(UInventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem))
		return;
	if (!OwningPlayerController->HasAuthority())
		return;
	if (!OwningSkeletalMesh.IsValid())
		return;

	FInventoryItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	if (auto* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInventoryItemEquipmentFragment>())
	{
		EquipmentFragment->OnEquip(OwningPlayerController.Get());

		AInventoryEquipActor* SpawnedActor = SpawnEquippedActor(*EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get());
		if (SpawnedActor)
		{
			EquippedActors.Emplace(SpawnedActor);
		}
	}
}

void UInventoryEquipmentComponent::OnItemUnequipped(UInventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem))
		return;
	if (!OwningPlayerController->HasAuthority())
		return;

	FInventoryItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	if (auto* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInventoryItemEquipmentFragment>())
	{
		EquipmentFragment->OnUnequip(OwningPlayerController.Get());
	}
}


