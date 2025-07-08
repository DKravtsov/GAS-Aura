// Copyright 4sandwiches


#include "InventoryManagement/Components/InventoryComponent.h"

#include "Inventory.h"
#include "Components/CapsuleComponent.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
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

void UInventoryComponent::OnRep_InventoryStorage()
{
	LOG_NETFUNCTIONCALL_COMPONENT
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
	LOG_NETFUNCTIONCALL_COMPONENT
	
	check(ItemComponent != nullptr);
	//FInventorySlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);
	FInventorySlotAvailabilityResult Result;
	if (!InventoryStorage->HasRoomForItem(Result, ItemComponent))
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

void UInventoryComponent::TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag)
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	checkf(OwningPlayerController->IsLocalController(), TEXT("This method should run only for local Player Controller"));
	
	FInventorySlotAvailabilityResult Result;
	if (!InventoryStorage->HasRoomForItem(Result, ItemManifest, StackCount))
	{
		OnNoRoomInInventory.Broadcast();
		return ;
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
		Server_AddStacksToItemAtStart(ItemManifest, Result.TotalRoomToFill, EquipmentTypeTag);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		Server_AddNewStartupItem(ItemManifest, Result.bStackable ? Result.TotalRoomToFill : 1, EquipmentTypeTag);
	}
}

FInventorySlotAvailabilityResult UInventoryComponent::ServerCheckHasRoomForItem(const FInventoryItemManifest& ItemManifest, int32 StackCountOverride) const
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	FInventorySlotAvailabilityResult Result;
	InventoryStorage->HasRoomForItem(Result, ItemManifest, StackCountOverride);
	return Result;
}

void UInventoryComponent::Server_TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag)
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	checkf(OwningPlayerController->HasAuthority(), TEXT("This method should run only on server"));

	
	FInventorySlotAvailabilityResult Result = ServerCheckHasRoomForItem(ItemManifest, StackCount);
	
	if (Result.TotalRoomToFill == 0)
	{
		OnNoRoomInInventory.Broadcast();
		return ;
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
		Server_AddStacksToItemAtStart(ItemManifest, Result.TotalRoomToFill, EquipmentTypeTag);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		Server_AddNewStartupItem(ItemManifest, Result.bStackable ? Result.TotalRoomToFill : 1, EquipmentTypeTag);
	}

}

void UInventoryComponent::Server_AddNewItem_Implementation(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
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
		ItemComponent->SetStackCount(Remainder);
		StackableFragment->SetStackCount(Remainder);
	}
}

bool UInventoryComponent::Server_AddNewItem_Validate(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	return true;
}

void UInventoryComponent::Server_AddNewStartupItem_Implementation(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag)
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	const auto NewItem = InventoryList.AddItem(ItemManifest, StackCount);
	check(NewItem != nullptr);
	NewItem->SetTotalStackCount(StackCount);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}
	if (EquipmentTypeTag.IsValid())
	{
		StartupEquipment.Emplace(NewItem, EquipmentTypeTag);
	}
}

bool UInventoryComponent::Server_AddNewStartupItem_Validate(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag)
{
	return true;
}

void UInventoryComponent::Server_AddStacksToItem_Implementation(UInventoryItemComponent* ItemComponent,	int32 StackCount, int32 Remainder)
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
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

void UInventoryComponent::Server_AddStacksToItemAtStart_Implementation(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag)
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	check(GetOwner()->HasAuthority());
	const FGameplayTag& ItemType = ItemManifest.GetItemType();
	if (UInventoryItem* Item = InventoryList.FindFirstItemByType(ItemType))
	{
		Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

		if (EquipmentTypeTag.IsValid())
		{
			StartupEquipment.Emplace(Item, EquipmentTypeTag);
		}
	}
}
bool UInventoryComponent::Server_AddStacksToItemAtStart_Validate(const FInventoryItemManifest& ItemManifest, int32 StackCount, const FGameplayTag& EquipmentTypeTag)
{
	return true;
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
	//DOREPLIFETIME(UInventoryComponent, InventoryStorage);
}

bool UInventoryComponent::TryEquipItem(UInventoryItem* ItemToEquip, const FGameplayTag& EquipmentTypeTag) const
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	return InventoryMenu->TryEquipItem(ItemToEquip, EquipmentTypeTag);
}

void UInventoryComponent::Client_ReceivedStartupInventory_Implementation()
{
	if (!bStartupItemsInitialized)
		return;
	
	LOG_NETFUNCTIONCALL_COMPONENT

	// recombine on the client the array with items that need to be equipped
	if (OwningPlayerController->HasAuthority())
		return;

	for (const auto& Item : StartupInventoryItems)
	{
		if (!Item.ShouldEquipToSlot.IsValid())
			continue;
		
		// in theory, this should be loaded at the moment, so, it should be no additional time spent here
		if (const auto* ItemData = Item.InventoryItem.LoadSynchronous())
		{
			if (ItemData->GetItemManifest().GetFragmentOfType<FInventoryItemStackableFragment>())
				continue;

			if (auto* InventoryItem = InventoryList.FindFirstItemByType(ItemData->GetItemManifest().GetItemType()))
			{
				StartupEquipment.Emplace(InventoryItem, Item.ShouldEquipToSlot);
			}
		}
	}
	
}

void UInventoryComponent::BeginPlay()
{
	LOG_NETFUNCTIONCALL_COMPONENT

	if (GetOwner()->HasAuthority())
	{
		if (ensure(IsValid(InventoryStorage)))
		{
			InventoryStorage->SetupStorage();
		}
	}
	
	Super::BeginPlay();

	OwningPlayerController = CastChecked<APlayerController>(GetOwner());
	InventoryController = OwningPlayerController->GetComponentByClass<UInventoryPlayerControllerComponent>();
	checkf(InventoryController != nullptr, TEXT("Player controller [%s] must have InventoryPlayerControllerComponent"),
		*OwningPlayerController->GetClass()->GetName());

	ConstructInventory();

	if (OwningPlayerController->IsLocalController() && !StartupInventoryItems.IsEmpty())
	{
		Server_AddStartupItems();
	}
}

void UInventoryComponent::Server_AddStartupItems_Implementation()
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	StartupEquipment.Reserve(StartupInventoryItems.Num());
		
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
			Server_TryAddStartupItem(ItemData->GetItemManifest(), StackCount, Item.ShouldEquipToSlot);
		}
	}
	bStartupItemsInitialized = true;
	Client_ReceivedStartupInventory();
}

bool UInventoryComponent::Server_AddStartupItems_Validate()
{
	return true;
}

// void UInventoryComponent::EquipStartupItems()
// {
// 	for (const auto& [ItemToEquip, EquipmentSlotTag] : StartupEquipment)
// 	{
// 		InventoryMenu->TryEquipItem(ItemToEquip.Get(), EquipmentSlotTag);
// 	}
// 	//StartupEquipment.Empty();
// }

void UInventoryComponent::ConstructInventory()
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
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
	InventoryMenu->OnOpenedMenu();
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

void UInventoryComponent::ReceivedStartupItems()
{
	if (!bStartupItemsInitialized)
	{
		bStartupItemsInitialized = true;
		Client_ReceivedStartupInventory();
	}
}

