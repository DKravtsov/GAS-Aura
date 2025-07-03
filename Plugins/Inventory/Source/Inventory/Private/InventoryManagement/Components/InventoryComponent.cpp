// Copyright 4sandwiches


#include "InventoryManagement/Components/InventoryComponent.h"

#include "Inventory.h"
#include "Components/CapsuleComponent.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "Player/InventoryPlayerControllerComponent.h"
#include "Items/InventoryItem.h"
#include "Items/InventoryItemData.h"
#include "Items/Fragments/InventoryItemFragment.h"

UInventoryComponent::UInventoryComponent()
	: InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventoryComponent::ToggleInventoryMenu()
{
	if (bInventoryMenuOpen)
	{
		CloseInventoryMenu();
	}
	else
	{
		OpenInventoryMenu();
	}
}

void UInventoryComponent::TryAddItem(UInventoryItemComponent* ItemComponent)
{
	check(ItemComponent != nullptr);
	FInventorySlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);
	if (Result.TotalRoomToFill == 0)
	{
		OnNoRoomInInventory.Broadcast();
		return;
	}

	UInventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item = FoundItem;

	if (Result.Item.IsValid() && Result.bStackable)
	{
		OnStackChanged.Broadcast(Result);
		// Add stacks to an item that already exists in the inventory.Only need to update the stack count
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 1, Result.Remainder);
	}
}

void UInventoryComponent::TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount)
{
	check(GetOwner()->HasAuthority());
	
	FInventorySlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemManifest, StackCount);
	if (Result.TotalRoomToFill == 0)
	{
		OnNoRoomInInventory.Broadcast();
		return;
	}
	UInventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemManifest.GetItemType());
	Result.Item = FoundItem;

	if (Result.Remainder > 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("Adding startup item [%s]: inventory has not enough room for %d more item(s) and they will be destroyed."),
		   *ItemManifest.GetItemType().ToString(), Result.Remainder);
	}

	if (Result.Item.IsValid() && Result.bStackable)
	{
		OnStackChanged.Broadcast(Result);
		// Add stacks to an item that already exists in the inventory.Only need to update the stack count
		Server_AddStacksToItemAtStart(ItemManifest, Result.TotalRoomToFill, Result.Remainder);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		Server_AddNewStartupItem(ItemManifest, Result.bStackable ? Result.TotalRoomToFill : 1, Result.Remainder);
	}
}

void UInventoryComponent::Server_AddNewItem_Implementation(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	const auto NewItem = InventoryList.AddItem(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}

	// tell the item component to destroy its owning actor if Remainder == 0
	//  otherwise, update the stack count for the pickup

	if (Remainder == 0)
	{
		ItemComponent->PickedUp();
	}
	else if (const auto StackableFragment = ItemComponent->GetItemManifestMutable().GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
}

bool UInventoryComponent::Server_AddNewItem_Validate(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	return true;
}

void UInventoryComponent::Server_AddNewStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, int32 Remainder)
{
	check(GetOwner()->HasAuthority());
	const auto NewItem = InventoryList.AddItem(ItemManifest, StackCount);
	NewItem->SetTotalStackCount(StackCount);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}
}

void UInventoryComponent::Server_AddStacksToItem_Implementation(UInventoryItemComponent* ItemComponent,	int32 StackCount, int32 Remainder)
{
	check(IsValid(ItemComponent));
	const FGameplayTag& ItemType = ItemComponent->GetItemManifest().GetItemType();
	if (UInventoryItem* Item = InventoryList.FindFirstItemByType(ItemType))
	{
		Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

		// tell the item component to destroy its owning actor if Remainder == 0
		//  otherwise, update the stack count for the pickup

		if (Remainder == 0)
		{
			ItemComponent->PickedUp();
		}
		else if (const auto StackableFragment = ItemComponent->GetItemManifestMutable().GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
		{
			StackableFragment->SetStackCount(Remainder);
		}
	}
}

bool UInventoryComponent::Server_AddStacksToItem_Validate(UInventoryItemComponent* ItemComponent, int32 StackCount,	int32 Remainder)
{
	return true;
}

void UInventoryComponent::Server_AddStacksToItemAtStart(const FInventoryItemManifest& ItemManifest, int32 StackCount, int32 Remainder) const
{
	check(GetOwner()->HasAuthority());
	const FGameplayTag& ItemType = ItemManifest.GetItemType();
	if (UInventoryItem* Item = InventoryList.FindFirstItemByType(ItemType))
	{
		Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);
	}
}


void UInventoryComponent::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
}

void UInventoryComponent::DropItem(UInventoryItem* Item, int32 StackCount)
{
	Server_DropItem(Item, StackCount);
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item, int32 StackCount)
{
	Server_ConsumeItem(Item, StackCount);
}

void UInventoryComponent::EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip)
{
	LOG_NETFUNCTIONCALL_COMPONENT_MSG(TEXT("Equip item [%s]; Unequip item [%s]"), *GetNameSafe(ItemToEquip), *GetNameSafe(ItemToUnequip));
	Server_EquipItem(ItemToEquip, ItemToUnequip);

	// This doesn't look right, so, I've commented out next lines. #todo need to check
	
	// if (OwningPlayerController->GetNetMode() != NM_DedicatedServer)
	// {
	// 	LOG_NETFUNCTIONCALL_COMPONENT_MSG(TEXT("    Broadcast OnEquipItem(%s); OnUnequipItem(%s)"), *GetNameSafe(ItemToEquip), *GetNameSafe(ItemToUnequip));
	// 	OnItemUnequipped.Broadcast(ItemToUnequip);
	// 	OnItemEquipped.Broadcast(ItemToEquip);
	// }
}

void UInventoryComponent::Server_EquipItem_Implementation(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip)
{
	LOG_NETFUNCTIONCALL_COMPONENT_MSG(TEXT("Equip item [%s]; Unequip item [%s]"), *GetNameSafe(ItemToEquip), *GetNameSafe(ItemToUnequip));
	Multicast_EquipItem(ItemToEquip, ItemToUnequip);
}

bool UInventoryComponent::Server_EquipItem_Validate(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip)
{
	return true;
}

void UInventoryComponent::Multicast_EquipItem_Implementation(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip)
{
	LOG_NETFUNCTIONCALL_COMPONENT_MSG(TEXT("Equip item [%s]; Unequip item [%s]"), *GetNameSafe(ItemToEquip), *GetNameSafe(ItemToUnequip));
	LOG_NETFUNCTIONCALL_COMPONENT_MSG(TEXT("    Broadcast OnEquipItem(%s); OnUnequipItem(%s)"), *GetNameSafe(ItemToEquip), *GetNameSafe(ItemToUnequip));
	OnItemUnequipped.Broadcast(ItemToUnequip);
	OnItemEquipped.Broadcast(ItemToEquip);
}

void UInventoryComponent::SpawnDroppedItem(UInventoryItem* Item, int32 StackCount)
{
	check(GetOwner()->HasAuthority());
	check(IsValid(Item));
	
	FVector SpawnLocation;
	FRotator SpawnRotation;
	GetDroppedItemSpawnLocationAndRotation(Item->GetItemType(),  SpawnLocation, SpawnRotation);

	FInventoryItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (const auto StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
	{
		// Note: we actually don't care about StackCount stored in a manifest until we pick up or drop an item.
		// So, we update it here to be copied into a new item if applicable.
		StackableFragment->SetStackCount(StackCount);
	}
	std::ignore = ItemManifest.SpawnPickupActor(GetOwner(), SpawnLocation, SpawnRotation);
}

void UInventoryComponent::Server_ConsumeItem_Implementation(UInventoryItem* Item, int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;

	if (NewStackCount <= 0)
	{
		InventoryList.RemoveItem(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}
	
	if (const auto ConsumableFragment = Item->GetItemManifest().GetFragmentOfType<FInventoryItemConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningPlayerController.Get());
	}
}

bool UInventoryComponent::Server_ConsumeItem_Validate(UInventoryItem* Item, int32 StackCount)
{
	return true;
}

void UInventoryComponent::GetDroppedItemSpawnLocationAndRotation_Implementation(const FGameplayTag& ItemType,
                                                                                FVector& SpawnLocation, FRotator& SpawnRotation)
{
	const APawn* OwningPawn = OwningPlayerController->GetPawn();
	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	SpawnLocation = OwningPawn->GetActorLocation() + RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);
	if (const auto CapsuleComp = OwningPawn->FindComponentByClass<UCapsuleComponent>())
	{
		SpawnLocation.Z -= CapsuleComp->GetScaledCapsuleHalfHeight();
	}
	SpawnRotation = FRotator::ZeroRotator;
}

void UInventoryComponent::Server_DropItem_Implementation(UInventoryItem* Item, int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;

	if (NewStackCount <= 0)
	{
		InventoryList.RemoveItem(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	SpawnDroppedItem(Item, StackCount);
}

bool UInventoryComponent::Server_DropItem_Validate(UInventoryItem* Item, int32 StackCount)
{
	return true;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayerController = CastChecked<APlayerController>(GetOwner());
	InventoryController = OwningPlayerController->GetComponentByClass<UInventoryPlayerControllerComponent>();
	checkf(InventoryController != nullptr, TEXT("Player controller [%s] must have InventoryPlayerControllerComponent"),
		*OwningPlayerController->GetClass()->GetName());

	ConstructInventory();

	if (GetOwner()->HasAuthority() && !StartupInventoryItems.IsEmpty())
	{
		// todo: make this operation async. We need to async load all item in one batch and then call this loop
		for (const auto& Item : StartupInventoryItems)
		{
			if (const auto* ItemData = Item.InventoryItem.LoadSynchronous())
			{
				int32 StackCount = -1;
				if (const auto* StackableFragment = ItemData->GetItemManifest().GetFragmentOfType<FInventoryItemStackableFragment>())
				{
					StackCount = Item.bOverrideCount
						? FMath::RandRange(Item.MinMaxAmount.X, Item.MinMaxAmount.Y)
						: StackableFragment->GetStackCount();
				}
				TryAddStartupItem(ItemData->GetItemManifest(), -1);
			}
		}
	}
}

void UInventoryComponent::ConstructInventory()
{
	if (!OwningPlayerController->IsLocalController())
		return;

	checkf(!InventoryMenuClass.IsNull(), TEXT("Forgot to set InventoryMenuClass in [%s|%s]"),
		*GetNameSafe(OwningPlayerController->GetClass()),
		*GetNameSafe(GetClass())
		);
	const TSubclassOf<UInventoryWidgetBase> LoadedInventoryMenuClass = InventoryMenuClass.LoadSynchronous();
	check(LoadedInventoryMenuClass);
	
	InventoryMenu = CreateWidget<UInventoryWidgetBase>(OwningPlayerController.Get(), LoadedInventoryMenuClass);
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
}

void UInventoryComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu))
		return;
	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bInventoryMenuOpen = true;

	// TODO Consider to choose input mode by parent project

	// For 3dr person template
	// if (OwningPlayerController.IsValid())
	// {
	// 	FInputModeGameAndUI InputMode;
	// 	OwningPlayerController->SetInputMode(InputMode);
	// 	OwningPlayerController->SetShowMouseCursor(true);
	// }

	// specific to Aura project
	if (OwningPlayerController.IsValid())
	{
		FInputModeUIOnly InputMode;
		OwningPlayerController->SetInputMode(InputMode);
	}
	OnInventoryMenuOpened.Broadcast();
}

void UInventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu))
		return;
	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	InventoryMenu->OnCloseMenu();
	bInventoryMenuOpen = false;


	// TODO Consider to choose input mode by parent project
	
	// For 3dr person template
	// if (OwningPlayerController.IsValid())
	// {
	// 	FInputModeGameOnly InputMode;
	// 	OwningPlayerController->SetInputMode(InputMode);
	// 	OwningPlayerController->SetShowMouseCursor(false);
	// }

	// specific to Aura project
	if (OwningPlayerController.IsValid())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		OwningPlayerController->SetInputMode(InputMode);
	}
	OnInventoryMenuClosed.Broadcast();
}
