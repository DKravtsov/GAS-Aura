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

void UInventoryEquipmentComponent::SetOwningSkeletalMesh(USkeletalMeshComponent* NewOwningSkeletalMesh)
{
	OwningSkeletalMesh = NewOwningSkeletalMesh;
}

void UInventoryEquipmentComponent::InitializeOwner(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		OwningPlayerController = PlayerController;
	}
	InitInventoryComponent();
}

AInventoryEquipActor* UInventoryEquipmentComponent::FindEquippedActorByEquipmentType(const FGameplayTag& EquipmentType) const
{
	if (const auto FoundActor = EquippedActors.Find(EquipmentType))
		return FoundActor->Get();
	return nullptr;
}

void UInventoryEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	InitPlayerController();
}

void UInventoryEquipmentComponent::InitPlayerController()
{
	if (OwningPlayerController = Cast<APlayerController>(GetOwner()); OwningPlayerController.IsValid())
	{
		if (ACharacter* OwnerCharacter = OwningPlayerController->GetCharacter())
		{
			OnPossessedPawnChanged(nullptr, OwnerCharacter);
		}
		else
		{
			OwningPlayerController->OnPossessedPawnChanged.AddDynamic(this, &UInventoryEquipmentComponent::OnPossessedPawnChanged);
		}
	}
}

void UInventoryEquipmentComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (const ACharacter* OwnerCharacter = OwningPlayerController->GetCharacter())
	{
		OwningSkeletalMesh = OwnerCharacter->GetMesh();
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

	//if (!bIsProxy)
	// {
	// 	InventoryComponent->EquipStartupItems(this);
	// }
	// for (const auto& [ItemToEquip, EquipmentSlotTag] : InventoryComponent->GetEquipStartupItems())
	// {
	// 	if (InventoryComponent->TryEquipItem(ItemToEquip.Get(), EquipmentSlotTag))
	// 	{
	// 		OnItemEquipped(ItemToEquip.Get());
	// 	}
	// }
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

void UInventoryEquipmentComponent::RemoveEquippedActorOfType(const FGameplayTag& EquipmentType)
{
	TObjectPtr<AInventoryEquipActor> EquippedActor = nullptr;
	if (EquippedActors.RemoveAndCopyValue(EquipmentType, EquippedActor))
	{
		EquippedActor->Destroy();
	}
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
		if (!bIsProxy)
		{
			EquipmentFragment->OnEquip(OwningPlayerController.Get());
		}

		if (AInventoryEquipActor* SpawnedActor = SpawnEquippedActor(*EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get()))
		{
			EquippedActors.Emplace(EquipmentFragment->GetEquipmentType(), SpawnedActor);
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
		if (!bIsProxy)
		{
			EquipmentFragment->OnUnequip(OwningPlayerController.Get());
		}
		
		RemoveEquippedActorOfType(EquipmentFragment->GetEquipmentType());
	}
}


