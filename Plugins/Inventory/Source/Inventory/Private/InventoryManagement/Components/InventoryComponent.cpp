// Copyright 4sandwiches


#include "InventoryManagement/Components/InventoryComponent.h"

#include "Components/CapsuleComponent.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "Player/InventoryPlayerControllerComponent.h"
#include "Items/InventoryItem.h"
#include "Items/InventoryItemData.h"
#include "Items/Fragments/InventoryItemFragment.h"

#include "DebugHelper.h"

UInventoryComponent::UInventoryComponent()
	: InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventoryComponent::OnRep_InventoryStorage()
{
	LOG_NETFUNCTIONCALL
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
	LOG_NETFUNCTIONCALL
	
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

/*void UInventoryComponent::TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot)
{
	LOG_NETFUNCTIONCALL
	
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
		Server_AddStacksToItemAtStart(ItemManifest, Result.TotalRoomToFill);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		Server_AddNewStartupItem(ItemManifest, Result.bStackable ? Result.TotalRoomToFill : 1, EquipToSlot);
	}
}*/

FInventorySlotAvailabilityResult UInventoryComponent::ServerCheckHasRoomForItem(const FInventoryItemManifest& ItemManifest, int32 StackCountOverride) const
{
	LOG_NETFUNCTIONCALL
	
	FInventorySlotAvailabilityResult Result;
	InventoryStorage->HasRoomForItem(Result, ItemManifest, StackCountOverride);
	return Result;
}

void UInventoryComponent::Server_TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot)
{
	LOG_NETFUNCTIONCALL
	
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
		Server_AddStacksToItemAtStart(ItemManifest, Result.TotalRoomToFill);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		Server_AddNewStartupItem(ItemManifest, Result.bStackable ? Result.TotalRoomToFill : 1, EquipToSlot);
	}

}

const FInventoryEquipmentSlot* UInventoryComponent::FindEquipmentSlotByEquippedItem(const UInventoryItem* Item) const
{
	if (IsValid(Item))
	{
		for (const auto& EquipSlot : EquipmentSlots)
		{
			if (EquipSlot.GetInventoryItem() == Item)
				return &EquipSlot;
		}
	}
	return nullptr;
}

FInventoryEquipmentSlot* UInventoryComponent::FindEquipmentSlotByEquippedItemMutable(const UInventoryItem* Item)
{
	return const_cast<FInventoryEquipmentSlot*>(FindEquipmentSlotByEquippedItem(Item));
}

FInventoryEquipmentSlot* UInventoryComponent::FindSuitableEquippedGridSlot(const FGameplayTag& ItemEquipmentTypeTag, bool bOnlyEmpty)
{
	return EquipmentSlots.FindByPredicate([ItemEquipmentTypeTag, bOnlyEmpty](const FInventoryEquipmentSlot& EquipmentSlot)
	{
		return (!bOnlyEmpty || EquipmentSlot.IsAvailable()) && ItemEquipmentTypeTag.MatchesTag(EquipmentSlot.GetEquipmentTypeTag());
	});
}

FInventoryEquipmentSlot* UInventoryComponent::GetEquipmentSlotMutable(EInventoryEquipmentSlot SlotId)
{
	return EquipmentSlots.FindByPredicate([SlotId](const FInventoryEquipmentSlot& EquipmentSlot)
	{
		return EquipmentSlot.GetSlotId() == SlotId;
	});
}

const FInventoryEquipmentSlot* UInventoryComponent::GetEquipmentSlot(EInventoryEquipmentSlot SlotId) const
{
	return EquipmentSlots.FindByPredicate([SlotId](const FInventoryEquipmentSlot& EquipmentSlot)
	{
		return EquipmentSlot.GetSlotId() == SlotId;
	});
}

void UInventoryComponent::Server_AddNewItem_Implementation(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	LOG_NETFUNCTIONCALL
	
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

void UInventoryComponent::Server_AddNewStartupItem_Implementation(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot)
{
	LOG_NETFUNCTIONCALL
	
	const auto NewItem = InventoryList.AddItem(ItemManifest, StackCount);
	check(NewItem != nullptr);
	NewItem->SetTotalStackCount(StackCount);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}
	if (EquipToSlot != EInventoryEquipmentSlot::Invalid)
	{
		if (GetEquipmentSlot(EquipToSlot))
		{
			StartupEquipment.Emplace(NewItem, EquipToSlot);
		}
	}
}

bool UInventoryComponent::Server_AddNewStartupItem_Validate(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot)
{
	return true;
}

void UInventoryComponent::Server_AddStacksToItem_Implementation(UInventoryItemComponent* ItemComponent,	int32 StackCount, int32 Remainder)
{
	LOG_NETFUNCTIONCALL
	
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

void UInventoryComponent::Server_AddStacksToItemAtStart_Implementation(const FInventoryItemManifest& ItemManifest, int32 StackCount)
{
	LOG_NETFUNCTIONCALL
	
	check(GetOwner()->HasAuthority());
	const FGameplayTag& ItemType = ItemManifest.GetItemType();
	if (UInventoryItem* Item = InventoryList.FindFirstItemByType(ItemType))
	{
		Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);
	}
}
bool UInventoryComponent::Server_AddStacksToItemAtStart_Validate(const FInventoryItemManifest& ItemManifest, int32 StackCount)
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

// void UInventoryComponent::EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip)
// {
// 	LOG_NETFUNCTIONCALL_MSG(TEXT("Equip item [%s]; Unequip item [%s]"), *GetNameSafe(ItemToEquip), *GetNameSafe(ItemToUnequip));
// 	Server_EquipItem(ItemToEquip, ItemToUnequip);
// }

void UInventoryComponent::EquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Equip item [%s] type [%s]; Unequip item [%s]"),
		*GetNameSafe(ItemToEquip), *UEnum::GetValueAsString(SlotId), *GetNameSafe(ItemToUnequip));
	
	if (!GetOwner()->HasAuthority())
	{
		// TODO: store transaction, we'll need this to confirm/revert after receiving the answer from the server
		// I guess, transactions should be also reverted by timeout if no response is received.
		// As an option, server can explicitly send Client_RejectEquip() ?
	}
	Server_EquipItem(ItemToEquip, ItemToUnequip, SlotId);
}

void UInventoryComponent::Client_RejectEquipItem_Implementation(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
{
}

void UInventoryComponent::Server_EquipItem_Implementation(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Equip item [%s] type [%s]; Unequip item [%s]"),
		*GetNameSafe(ItemToEquip), *UEnum::GetValueAsString(SlotId), *GetNameSafe(ItemToUnequip));

	// Todo: validate the ItemToEquip can be equipped (don't trust anyone)
	
	// Find the correct slot to equip the item
	FInventoryEquipmentSlot* EquippedSlot = GetEquipmentSlotMutable(SlotId);
	if (!ensure(EquippedSlot))
	{
		UE_LOG(LogInventory, Error, TEXT("Server_EquipItem: The Equipment slot not found: %s"), *UEnum::GetValueAsString(SlotId));
		return;
	}
	if (ItemToEquip)
	{
		const FGameplayTag& EquipmentType = UInventoryStatics::GetItemEquipmentTag(ItemToEquip);
		if (!EquipmentType.MatchesTag(EquippedSlot->GetEquipmentTypeTag()))
		{
			UE_LOG(LogInventory, Error, TEXT("Server_EquipItem: The Equipment slot [%s] not fit for Item's type: %s"),
				*UEnum::GetValueAsString(SlotId), *EquipmentType.ToString());
			return;
		}
	}

	// If the selected slot already has an equipped item, validate if it can be unequipped (and if it's ItemToUnequip?)
	UInventoryItem* CurrentEquippedItem = EquippedSlot->GetInventoryItem().Get();
	if (CurrentEquippedItem && CurrentEquippedItem == ItemToEquip)
	{
		UE_LOG(LogInventory, Log, TEXT("Server_EquipItem: Trying to equip already equipped item [%s] to slot [%s]"),
			*ItemToEquip->GetItemType().ToString(), *UEnum::GetValueAsString(SlotId));
		return;
	}
	if (CurrentEquippedItem != ItemToUnequip)
	{
		if (ItemToUnequip != nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("Server_EquipItem: Trying to unequip item [%s] that is not equipped in slot [%s]"),
			   *ItemToEquip->GetItemType().ToString(), *UEnum::GetValueAsString(SlotId));
			return;
		}
		else
		{
			// we might call this method with arg ItemToUnequip==null to "autodetect" the item to unequip
			ItemToUnequip = CurrentEquippedItem;
		}
	}

	// Before equipping:
	
	// Remove ItemToEquip from the inventory storage (if applicable).
	// Note: the item could be already removed from the storage if this command originally came from UI
	if (ItemToEquip)
	{
		const int32 ItemGridIndex = InventoryStorage->GetItemIndex(ItemToEquip);
		if (ItemGridIndex != INDEX_NONE)
		{
			InventoryStorage->RemoveItemFromGrid(ItemToEquip, ItemGridIndex);
		}
	}

	// Unequip the item:
	if (ItemToUnequip)
	{
		// Remove ItemToUnequip from the slot
		EquippedSlot->SetInventoryItem(nullptr);
		
		// Add ItemToUnequip to the inventory storage (if applicable).
		// Note: If the command originally came from UI, the Item shouldn't be returned to the storage,
		// because it's on hover item and will be returned separately. Probably, the best solution how to detect
		// this case is to have ItemToUnequip argument valid only in this case.
		
	}

	// Equip item
	if (ItemToEquip)
	{
		// Add item to the slot
		EquippedSlot->SetInventoryItem(ItemToEquip);
		
		// Apply EquipFragment modifiers
	}
	// Notify subscribers about equipping/unequipping fact
	Multicast_EquipItem(ItemToEquip, ItemToUnequip, SlotId);
}

bool UInventoryComponent::Server_EquipItem_Validate(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
{
	return true;
}

void UInventoryComponent::Multicast_EquipItem_Implementation(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("[Slot: %d] Broadcast OnEquipItem(%s); OnUnequipItem(%s)"),
	                                  static_cast<int32>(SlotId), *GetNameSafe(ItemToEquip), *GetNameSafe(ItemToUnequip));

	if (!GetOwner()->HasAuthority())
	{
		// TODO: check if the items to equip and unequip are the same as in the current transaction,
		// so we can revert if needed
	}

	OnItemUnequipped.Broadcast(ItemToUnequip);
	OnItemEquipped.Broadcast(ItemToEquip);
}

void UInventoryComponent::SpawnDroppedItem(UInventoryItem* Item, int32 StackCount)
{
	check(GetOwner()->HasAuthority());
	check(IsValid(Item));
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s], stack count [%d]"), *Item->GetItemType().ToString(), StackCount)
	
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
	check(IsValid(Item));
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s], stack count [%d]"), *Item->GetItemType().ToString(), StackCount)
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
	check(IsValid(Item));
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s], stack count [%d]"), *Item->GetItemType().ToString(), StackCount)

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
	DOREPLIFETIME(UInventoryComponent, InventoryStorage);
}

bool UInventoryComponent::TryEquipItem(UInventoryItem* ItemToEquip, EInventoryEquipmentSlot SlotId)
{
	check(IsValid(ItemToEquip));
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s], slot %d"), *GetInventoryItemId(ItemToEquip), static_cast<int32>(SlotId))

	if (SlotId == EInventoryEquipmentSlot::Invalid)
		return false;
	
	if (FindEquipmentSlotByEquippedItem(ItemToEquip))
		return false;// is already equipped

	const FGameplayTag EquipmentTypeTag = UInventoryStatics::GetItemEquipmentTag(ItemToEquip);
	if (!UInventoryStatics::CanEquipItem(ItemToEquip, EquipmentTypeTag))
		return false;

	const FInventoryEquipmentSlot* EquipSlot = GetEquipmentSlot(SlotId);
	if (!EquipSlot)
		return false;

	UInventoryItem* ItemToUnequip = nullptr;
	if (!EquipSlot->IsAvailable())
	{
		if (!EquipSlot->GetInventoryItem().IsValid())
			return false;
		ItemToUnequip = EquipSlot->GetInventoryItem().Get();
	}

	Server_EquipItem(ItemToEquip, ItemToUnequip, SlotId);
	
	return true;
}

void UInventoryComponent::Client_ReceivedStartupInventory_Implementation()
{
	if (!bStartupItemsInitialized)
		return;
	
	LOG_NETFUNCTIONCALL

	// recombine on the client the array with items that need to be equipped
	if (OwningPlayerController->HasAuthority())
		return;

	for (const auto& Item : StartupInventoryItems)
	{
		if (!Item.bShouldEquip)
			continue;
		
		// in theory, this should be loaded at the moment, so, it should be no additional time spent here
		if (const auto* ItemData = Item.InventoryItem.LoadSynchronous())
		{
			if (ItemData->GetItemManifest().GetFragmentOfType<FInventoryItemStackableFragment>())
				continue;

			if (auto* InventoryItem = InventoryList.FindFirstItemByType(ItemData->GetItemManifest().GetItemType()))
			{
				const auto* EquipSlot = FindEquipmentSlotByEquippedItem(InventoryItem);
				if (EquipSlot == nullptr)
					continue;
				StartupEquipment.Emplace(InventoryItem, EquipSlot->GetSlotId());
			}
		}
	}
}

void UInventoryComponent::CreateInventoryStorage()
{
	check(!IsValid(InventoryStorage));
	check(InventoryStorageSetupData.IsValid());

	InventoryStorage = NewObject<UInventoryStorage>(this, InventoryStorageSetupData.Get<FInventoryStorageSetupData>().StorageClass);	
	check(InventoryStorage);
	InventoryStorage->SetupStorage(InventoryStorageSetupData);
}

void UInventoryComponent::BeginPlay()
{
	LOG_NETFUNCTIONCALL

	if (GetOwner()->HasAuthority())
	{
		CreateInventoryStorage();
	}
	
	Super::BeginPlay();

	OwningPlayerController = CastChecked<APlayerController>(GetOwner());
	InventoryController = OwningPlayerController->GetComponentByClass<UInventoryPlayerControllerComponent>();
	checkf(InventoryController != nullptr, TEXT("Player controller [%s] must have InventoryPlayerControllerComponent"),
		*OwningPlayerController->GetClass()->GetName());

	if (GetOwner()->HasAuthority())
	{
		ConstructInventoryMenu();

		if (OwningPlayerController->IsLocalController() && !StartupInventoryItems.IsEmpty())
		{
			Server_AddStartupItems();
		}
	}
}

EInventoryEquipmentSlot UInventoryComponent::GetValidEquipSlotId(const FInventoryItemProxy& Item, const FInventoryItemManifest& ItemManifest)
{
	EInventoryEquipmentSlot EquipSlotId = EInventoryEquipmentSlot::Invalid;
	while (Item.bShouldEquip)
	{
		if (!ItemManifest.GetItemCategory().MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment)
			|| ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>())
		{
			break;
		}
				
		if (const auto* EquipFragment = ItemManifest.GetFragmentOfType<FInventoryItemEquipmentFragment>())
		{
			if (Item.EquipmentSlot == EInventoryEquipmentSlot::Invalid)
			{
				// try to find a suitable one
				if (const auto EquipSlot = FindSuitableEquippedGridSlot(EquipFragment->GetEquipmentType()))
				{
					EquipSlotId = EquipSlot->GetSlotId();
					break;
				}
			}
			else if (const auto EquipSlot = GetEquipmentSlot(Item.EquipmentSlot))
			{
				if (EquipFragment->GetEquipmentType().MatchesTag(EquipSlot->GetEquipmentTypeTag()) && EquipSlot->IsAvailable())
				{
					EquipSlotId = Item.EquipmentSlot;
					break;
				}
			}
		}
		break;
	}
	return EquipSlotId;
}

void UInventoryComponent::Server_AddStartupItems_Implementation()
{
	LOG_NETFUNCTIONCALL
	
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
			EInventoryEquipmentSlot EquipSlotId = GetValidEquipSlotId(Item, ItemData->GetItemManifest());
			Server_TryAddStartupItem(ItemData->GetItemManifest(), StackCount, EquipSlotId);
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

void UInventoryComponent::OnRep_StorageSetupData()
{
	LOG_NETFUNCTIONCALL

	CreateInventoryStorage();
}

void UInventoryComponent::ConstructInventoryMenu()
{
	if (!OwningPlayerController->IsLocalController())
		return;

	LOG_NETFUNCTIONCALL
	
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
		LOG_NETFUNCTIONCALL
		
		bStartupItemsInitialized = true;
		Client_ReceivedStartupInventory();
	}
}

//#if UE_WITH_CHEAT_MANAGER
void UInventoryComponent::DebugPrintStorage() const
{
	if (InventoryStorage)
	{
		InventoryStorage->DebugPrintStorage();
	}
	else
	{
		DebugPrint(TEXT("DebugPrintStorage: Inventory Storage is null"), FColor::Red);
	}

	FStringBuilderBase Output;
	Output += TEXT("Equipment slots:\n");
	for (const auto& EquipSlot : EquipmentSlots)
	{
		Output.Appendf(TEXT("Slot %d (%s): %s\n"), static_cast<int32>(EquipSlot.GetSlotId()),
			*EquipSlot.GetEquipmentTypeTag().ToString(), *GetInventoryItemId(EquipSlot.GetInventoryItem().Get()));
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), Output.ToString());
}
//#endif//UE_WITH_CHEAT_MANAGER
