// Copyright 4sandwiches


#include "EquipmentManagement/Components/InventoryEquipmentComponent.h"

#include "Inventory.h"
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
	LOG_NETFUNCTIONCALL_COMPONENT_MSG(TEXT(" OwningPC: %s"), *PlayerController->GetName())
	
	if (IsValid(PlayerController))
	{
		OwningPlayerController = PlayerController;
		InitInventoryComponent();
	}
}

AInventoryEquipActor* UInventoryEquipmentComponent::FindEquippedActorByEquipmentType(const FGameplayTag& EquipmentType) const
{
	if (const auto FoundActor = EquippedActors.Find(EquipmentType))
		return FoundActor->Get();
	return nullptr;
}

void UInventoryEquipmentComponent::BeginPlay()
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	Super::BeginPlay();
	InitPlayerController();
}

void UInventoryEquipmentComponent::InitPlayerController()
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
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
	LOG_NETFUNCTIONCALL_COMPONENT
	
	if (const ACharacter* OwnerCharacter = OwningPlayerController->GetCharacter())
	{
		OwningSkeletalMesh = OwnerCharacter->GetMesh();
		InitInventoryComponent();
	}
}

void UInventoryEquipmentComponent::InitInventoryComponent()
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
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

	if (OwningPlayerController->IsLocalController() && !bIsProxy)
	{
		WaitForStartupEquipmentReady();
	}
	if (OwningPlayerController->IsLocalController() && bIsProxy && InventoryComponent->AreStartupItemsEquipped())
	{
		// the component initialized late, so all startup items already equipped.
		// we need to reflect this on the proxy mesh actor only
		InitStartupEquipment();
	}
}

void UInventoryEquipmentComponent::InitStartupEquipment()
{
	LOG_NETFUNCTIONCALL_COMPONENT

	if (!bIsProxy)
	{
		for (const auto& [ItemToEquip, EquipmentSlotTag] : InventoryComponent->GetEquipStartupItems())
		{
			if (InventoryComponent->TryEquipItem(ItemToEquip.Get(), EquipmentSlotTag))
			{
				OnItemEquipped(ItemToEquip.Get());
			}
		}
		InventoryComponent->ReceivedStartupItemsEquipped();
	}
	else
	{
		for (const auto& [ItemToEquip, EquipmentSlotTag] : InventoryComponent->GetEquipStartupItems())
		{
			if (InventoryComponent->TryEquipItem(ItemToEquip.Get(), EquipmentSlotTag))
			{
				OnItemEquipped(ItemToEquip.Get());
			}
		}
	}
}

void UInventoryEquipmentComponent::WaitForStartupEquipmentReady()
{
	check(OwningPlayerController.IsValid() && InventoryComponent.IsValid());
	if (InventoryComponent->AreStartupItemsInitialized())
	{
		InitStartupEquipment();
	}
	else
	{
		OwningPlayerController->GetWorldTimerManager().SetTimerForNextTick(this, &UInventoryEquipmentComponent::WaitForStartupEquipmentReady);
	}
}

AInventoryEquipActor* UInventoryEquipmentComponent::SpawnEquippedActor(
	FInventoryItemEquipmentFragment& EquipmentFragment,
	const FInventoryItemManifest& ItemManifest,
	USkeletalMeshComponent* ParentMesh)
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	check(OwningPlayerController.IsValid());
	const bool bIsClient = GetWorld()->GetNetMode() == NM_Client;
	
	AInventoryEquipActor* SpawnedActor = EquipmentFragment.SpawnEquippedActor(ParentMesh);
	if (SpawnedActor)
	{
		SpawnedActor->SetOwner(GetOwner());//OwningPlayerController.Get());
		if (bIsProxy)
		{
			SpawnedActor->SetReplicates(false);
		}
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
	LOG_NETFUNCTIONCALL_COMPONENT
	
	if (!IsValid(EquippedItem))
		return;
	// if (!GetOwner()->HasAuthority())
	// 	return;
	if (!OwningSkeletalMesh.IsValid())
		return;
	const bool bIsClient = GetWorld()->GetNetMode() == NM_Client;

	FInventoryItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	if (auto* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInventoryItemEquipmentFragment>())
	{
		if (!bIsProxy && !bIsClient)
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
	LOG_NETFUNCTIONCALL_COMPONENT
	
	if (!IsValid(UnequippedItem))
		return;
	// if (!GetOwner()->HasAuthority())
	// 	return;

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


