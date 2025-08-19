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
#include "InventoryManagement/Data/InventorySetupData.h"

#include "DebugHelper.h"

const FInventoryStorageSetupData* FStorageSetupDataProxy::GetData() const
{
	return SetupData.GetPtr<FInventoryStorageSetupData>();
}

TSubclassOf<UInventoryStorage> FStorageSetupDataProxy::GetStorageClass() const
{
	const auto Data = GetData();
	return Data ? Data->StorageClass : nullptr;
}

UInventoryComponent::UInventoryComponent()
	: InventoryList(this)
	, EquipmentSlots(this) 
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
	LOG_NETFUNCTIONCALL
	
	check(ItemComponent != nullptr);

	if (OwningPlayerController->GetLocalRole() == ROLE_AutonomousProxy /*!OwningPlayerController->HasAuthority() && OwningPlayerController->IsLocalPlayerController()*/)
	{
		// Precheck to not call server RPC if it's unnecessary.
		// Server will check the same anyway, so it doesn't make sense to call this check if HasAuthority.
		// We assume here that the inventory grid is fully synced at this moment 
		FInventorySlotAvailabilityResult Result;
		if (!InventoryStorage->HasRoomForItem(Result, ItemComponent))
		{
			OnNoRoomInInventory.Broadcast();
			return;
		}
	}

	Server_TryAddItem(ItemComponent);
}

void UInventoryComponent::Server_TryAddItem_Implementation(UInventoryItemComponent* ItemComponent)
{
	LOG_NETFUNCTIONCALL
	
	check(ItemComponent != nullptr);

	FInventorySlotAvailabilityResult Result;
	if (!InventoryStorage->HasRoomForItem(Result, ItemComponent))
	{
		if (OwningPlayerController->IsLocalPlayerController())
		{
			// The client broadcasts this in TryAddItem()
			OnNoRoomInInventory.Broadcast();
		}
		return;
	}

	UInventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item = FoundItem;

	if (Result.Item.IsValid() && Result.bStackable)
	{
		OnStackChanged.Broadcast(Result);
		// Add stacks to an item that already exists in the inventory.Only need to update the stack count
		AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 1, Result.Remainder);
	}
}

bool UInventoryComponent::Server_TryAddItem_Validate(UInventoryItemComponent* ItemComponent)
{
	return true;
}

void UInventoryComponent::TryAddStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot, TArray<FInventoryStartupEquipmentData>& OutStartupEquipmentArray)
{
	LOG_NETFUNCTIONCALL
	
	checkf(OwningPlayerController->HasAuthority(), TEXT("This method should run only on server"));

	FInventorySlotAvailabilityResult Result;
	if (!InventoryStorage->HasRoomForItem(Result, ItemManifest, StackCount))
	{
		UE_LOG(LogInventory, Error, TEXT("Adding startup item [%s]: inventory has no room for %d items."),
			*ItemManifest.GetItemType().ToString(), StackCount);
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
		AddStacksToItemAtStart(ItemManifest, Result.TotalRoomToFill);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		AddNewStartupItem(ItemManifest, Result.bStackable ? Result.TotalRoomToFill : 1, EquipToSlot, OutStartupEquipmentArray);
	}

}

const FInventoryEquipmentSlot* UInventoryComponent::FindEquipmentSlotByEquippedItem(const UInventoryItem* Item) const
{
	if (IsValid(Item))
	{
		return EquipmentSlots.GetSlotByItem(Item);
	}
	return nullptr;
}

EInventoryEquipmentSlot UInventoryComponent::FindSuitableEquippedGridSlot(const FGameplayTag& ItemEquipmentTypeTag, bool bOnlyEmpty) const
{
	auto* EquipSlot = EquipmentSlots.FindByPredicate([ItemEquipmentTypeTag, bOnlyEmpty](const FInventoryEquipmentSlot& EquipmentSlot)
	{
		return (!bOnlyEmpty || EquipmentSlot.IsAvailable()) && ItemEquipmentTypeTag.MatchesTag(EquipmentSlot.GetEquipmentTypeTag());
	});
	return EquipSlot ? EquipSlot->GetSlotId() : EInventoryEquipmentSlot::Invalid;
}

FInventoryEquipmentSlot* UInventoryComponent::GetEquipmentSlotMutable(EInventoryEquipmentSlot SlotId)
{
	return EquipmentSlots.GetSlotByIdMutable(SlotId);
}

const FInventoryEquipmentSlot* UInventoryComponent::GetEquipmentSlot(EInventoryEquipmentSlot SlotId) const
{
	return EquipmentSlots.GetSlotById(SlotId);
}

void UInventoryComponent::AddNewItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	check(GetOwner()->HasAuthority());

	LOG_NETFUNCTIONCALL
	
	const auto NewItem = InventoryList.AddItem(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);
	if (const auto StackableFragment = NewItem->GetItemManifestMutable().GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}

	OnItemAdded.Broadcast(NewItem);

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

void UInventoryComponent::AddNewStartupItem(const FInventoryItemManifest& ItemManifest, int32 StackCount, EInventoryEquipmentSlot EquipToSlot, TArray<FInventoryStartupEquipmentData>& OutStartupEquipmentArray)
{
	LOG_NETFUNCTIONCALL
	
	check(GetOwner()->HasAuthority());

	const auto NewItem = InventoryList.AddItem(ItemManifest, StackCount);
	check(NewItem != nullptr);
	NewItem->SetTotalStackCount(StackCount);
	if (const auto StackableFragment = NewItem->GetItemManifestMutable().GetFragmentOfTypeMutable<FInventoryItemStackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}

	OnItemAdded.Broadcast(NewItem);

	if (EquipToSlot != EInventoryEquipmentSlot::Invalid)
	{
		if (GetEquipmentSlot(EquipToSlot))
		{
			OutStartupEquipmentArray.Emplace(NewItem, EquipToSlot);
		}
	}
}

void UInventoryComponent::AddStacksToItem(UInventoryItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	LOG_NETFUNCTIONCALL

	check(GetOwner()->HasAuthority());
	
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

void UInventoryComponent::AddStacksToItemAtStart(const FInventoryItemManifest& ItemManifest, int32 StackCount)
{
	LOG_NETFUNCTIONCALL
	
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

void UInventoryComponent::ConsumeItem(UInventoryItem* Item, int32 GridIndex, int32 StackCount)
{
	Server_ConsumeItem(Item, GridIndex, StackCount);
}

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
	// TODO: review the comments below, they seem out-of-date
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
		UE_LOG(LogInventory, Log, TEXT("Server_EquipItem: Trying to equip already equipped item [%s] to slot %d"),
			*ItemToEquip->GetItemType().ToString(), static_cast<int32>(SlotId));
		return;
	}
	if (CurrentEquippedItem != ItemToUnequip)
	{
		if (ItemToUnequip != nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("Server_EquipItem: Trying to unequip item [%s] that is not equipped in slot %d"),
			   *ItemToUnequip->GetItemType().ToString(), static_cast<int32>(SlotId));
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
			RemoveItemFromStorage(ItemToEquip, ItemGridIndex);
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

void UInventoryComponent::BroadcastEquipItem(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
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

void UInventoryComponent::Multicast_EquipItem_Implementation(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
{
	LOG_NETFUNCTIONCALL
	
	BroadcastEquipItem(ItemToEquip, ItemToUnequip, SlotId);
}

void UInventoryComponent::Client_EquipItem_Implementation(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId)
{
	LOG_NETFUNCTIONCALL
	
	BroadcastEquipItem(ItemToEquip, ItemToUnequip, SlotId);
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

void UInventoryComponent::Server_ConsumeItem_Implementation(UInventoryItem* Item, int32 GridIndex, int32 StackCount)
{
	check(IsValid(Item));
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s], stack count [%d]"), *Item->GetItemType().ToString(), StackCount)
	const int32 NewTotalStackCount = Item->GetTotalStackCount() - StackCount;

	if (NewTotalStackCount <= 0)
	{
		InventoryList.RemoveItem(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewTotalStackCount);
		const int32 NewStackCount = InventoryStorage->GetItemStackCount(Item, GridIndex) - StackCount;
		InventoryStorage->SetItemStackCount(Item, GridIndex, NewStackCount);
	}
	
	if (const auto ConsumableFragment = Item->GetItemManifest().GetFragmentOfType<FInventoryItemConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningPlayerController.Get());
	}
}

bool UInventoryComponent::Server_ConsumeItem_Validate(UInventoryItem* Item, int32 GridIndex, int32 StackCount)
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
	DOREPLIFETIME(UInventoryComponent, EquipmentSlots);
}

void UInventoryComponent::TryEquipItem(UInventoryItem* ItemToEquip, EInventoryEquipmentSlot SlotId)
{
	check(IsValid(ItemToEquip));
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s], slot %d"), *GetInventoryItemId(ItemToEquip), static_cast<int32>(SlotId))

	if (SlotId == EInventoryEquipmentSlot::Invalid)
		return;
	
	if (FindEquipmentSlotByEquippedItem(ItemToEquip))
		return;// is already equipped

	const FGameplayTag EquipmentTypeTag = UInventoryStatics::GetItemEquipmentTag(ItemToEquip);
	if (!UInventoryStatics::CanEquipItem(ItemToEquip, EquipmentTypeTag))
		return;

	const FInventoryEquipmentSlot* EquipSlot = GetEquipmentSlot(SlotId);
	if (!EquipSlot)
		return;

	UInventoryItem* ItemToUnequip = nullptr;
	if (!EquipSlot->IsAvailable())
	{
		if (!EquipSlot->GetInventoryItem().IsValid())
			return;
		ItemToUnequip = EquipSlot->GetInventoryItem().Get();
	}

	Server_EquipItem(ItemToEquip, ItemToUnequip, SlotId);
	
}

void UInventoryComponent::CreateInventoryStorage()
{
	LOG_NETFUNCTIONCALL
	
	if (!ensure(!IsValid(InventoryStorage)))
		return;

	check(InventorySetupData.IsValid());

	const FInventoryStorageSetupData* SetupData = InventorySetupData->InventoryStorage.GetData();
	check(SetupData != nullptr);

	InventoryStorage = NewObject<UInventoryStorage>(this, SetupData->StorageClass);	
	check(InventoryStorage);
	AddRepSubObj(InventoryStorage);

	if (GetOwner()->HasAuthority())
	{
		InventoryStorage->SetupStorage(SetupData);

		for (const auto& [SlotId, EquipmentTypeTag] : InventorySetupData->EquipmentSlots)
		{
			EquipmentSlots.AddSlot(SlotId, EquipmentTypeTag);
		}
	}
}

void UInventoryComponent::SetOwnerInternal()
{
	if (OwningPlayerController.IsValid())
		return;

	LOG_NETFUNCTIONCALL
	
	OwningPlayerController = CastChecked<APlayerController>(GetOwner());
	InventoryController = OwningPlayerController->GetComponentByClass<UInventoryPlayerControllerComponent>();
	checkf(InventoryController != nullptr, TEXT("Player controller [%s] must have InventoryPlayerControllerComponent"),
	       *OwningPlayerController->GetClass()->GetName());
}

void UInventoryComponent::BeginPlay()
{
	LOG_NETFUNCTIONCALL

	if (!InventorySetupData.IsNull())
	{
		// force loading
		InventorySetupData.LoadSynchronous();
	}

	if (GetOwner()->HasAuthority())
	{
		CreateInventoryStorage();
	}
	
	Super::BeginPlay();

	SetOwnerInternal();

	if (OwningPlayerController->IsLocalController())
	{
		ConstructInventoryMenu();

		if (!InventorySetupData->DefaultStartupInventory.IsEmpty())
		{
			Server_AddStartupItems();
		}
	}

}

EInventoryEquipmentSlot UInventoryComponent::GetValidEquipSlotId(EInventoryEquipmentSlot DesiredSlotId, const FInventoryItemManifest& ItemManifest)
{
	EInventoryEquipmentSlot EquipSlotId = EInventoryEquipmentSlot::Invalid;
	while (true)
	{
		if (!ItemManifest.GetItemCategory().MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment)
			|| ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>())
		{
			break;
		}
				
		if (const auto* EquipFragment = ItemManifest.GetFragmentOfType<FInventoryItemEquipmentFragment>())
		{
			if (DesiredSlotId == EInventoryEquipmentSlot::Invalid)
			{
				// try to find a suitable one
				const EInventoryEquipmentSlot FoundSlot = FindSuitableEquippedGridSlot(EquipFragment->GetEquipmentType());
				if (FoundSlot != EInventoryEquipmentSlot::Invalid)
				{
					EquipSlotId = FoundSlot;
					break;
				}
			}
			else if (const auto EquipSlot = GetEquipmentSlot(DesiredSlotId))
			{
				if (EquipFragment->GetEquipmentType().MatchesTag(EquipSlot->GetEquipmentTypeTag()) && EquipSlot->IsAvailable())
				{
					EquipSlotId = DesiredSlotId;
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

	const TArray<FInventoryItemProxy>& StartupInventoryItems = InventorySetupData->DefaultStartupInventory;
	
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
			const EInventoryEquipmentSlot EquipSlotId = Item.bShouldEquip ?
				GetValidEquipSlotId(Item.EquipmentSlot, ItemData->GetItemManifest()) : EInventoryEquipmentSlot::Invalid;
			TryAddStartupItem(ItemData->GetItemManifest(), StackCount, EquipSlotId, StartupEquipment);
		}
	}
	bStartupItemsInitialized = true;
}

bool UInventoryComponent::Server_AddStartupItems_Validate()
{
	return true;
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
		Server_RequestStartupEquipment();
	}
}

void UInventoryComponent::Server_RequestStartupEquipment_Implementation()
{
	LOG_NETFUNCTIONCALL

	Client_EquipStartupInventory(StartupEquipment);
}

void UInventoryComponent::Client_EquipStartupInventory_Implementation(const TArray<FInventoryStartupEquipmentData>& StartupEquipmentData)
{
	// resend equip item events
	for (const auto& [Item, SlotId] : StartupEquipmentData)
	{
		Client_EquipItem(Item.Get(), nullptr, SlotId);
	}
}

void UInventoryComponent::ClearSelectedItem()
{
	HoverItemProxy.Reset();

	OnHoverItemReset.Broadcast();

	if (!OwningPlayerController->IsLocalController() && OwningPlayerController->HasAuthority())
	{
		Client_ReceivedHoverItemReset();
	}
}

void UInventoryComponent::Client_ReceivedHoverItemReset_Implementation()
{
	ClearSelectedItem();
}

void UInventoryComponent::Server_FillInStacksOrConsumeHover_Implementation(UInventoryItem* Item, int32 GridIndex)
{
	LOG_NETFUNCTIONCALL

	if (!IsValid(Item) || !Item->IsStackable())
		return;

	if (!HasHoverItem() || HoverItemProxy->InventoryItem != Item)
	{
		UE_LOG(LogInventory, Error, TEXT("Only the stacks of the same item can be swapped. Expected [%s], got [%s]"),
			*GetInventoryItemId(Item), *GetInventoryItemId(HasHoverItem() ? HoverItemProxy->InventoryItem.Get() : nullptr))
		return;
	}
	const int32 Remainder = InventoryStorage->FillInStacksOrConsumeHover(Item, GridIndex, HoverItemProxy->StackCount);
	if (Remainder == 0)
	{
		ClearSelectedItem();
	}
	else
	{
		HoverItemProxy->StackCount = Remainder;
		NotifyHoverItemUpdated();
	}
}

bool UInventoryComponent::Server_FillInStacksOrConsumeHover_Validate(UInventoryItem* Item, int32 GridIndex)
{
	return true;
}

void UInventoryComponent::Server_SwapStackCountWithHoverItem_Implementation(UInventoryItem* Item, int32 GridIndex)
{
	LOG_NETFUNCTIONCALL

	if (!IsValid(Item) || !Item->IsStackable())
	{
		UE_LOG(LogInventory, Error, TEXT("Only stackable items can swap stacks. [%s] index: %d"), *GetInventoryItemId(Item), GridIndex)
		return;
	}
	
	if (!HasHoverItem() || HoverItemProxy->InventoryItem != Item)
	{
		UE_LOG(LogInventory, Error, TEXT("Only the stacks of the same item can be swapped. Expected [%s], got [%s]"),
			*GetInventoryItemId(Item), *GetInventoryItemId(HasHoverItem() ? HoverItemProxy->InventoryItem.Get() : nullptr))
		return;
	}

	int32 StackCount =  GetInventoryStorage()->GetItemStackCount(Item, GridIndex);
	GetInventoryStorage()->SetItemStackCount(Item, GridIndex, HoverItemProxy->StackCount);
	HoverItemProxy->StackCount = StackCount;
	NotifyHoverItemUpdated();
}

bool UInventoryComponent::Server_SwapStackCountWithHoverItem_Validate(UInventoryItem* Item, int32 GridIndex)
{
	return true;
}

void UInventoryComponent::Server_SwapSelectedWitItem_Implementation(UInventoryItem* ItemOnGrid, int32 GridIndex)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s] index: %d"), *GetInventoryItemId(ItemOnGrid), GridIndex)

	if (!IsValid(ItemOnGrid) || !HasHoverItem())
		return;

	FHoverItemProxy OldHoverItem = HoverItemProxy.GetValue();
	UInventoryItem* HoverItem = OldHoverItem.InventoryItem.Get();

	// keep the same PreviousGridIndex
	Server_SelectItem(ItemOnGrid, GridIndex, OldHoverItem.PreviousIndex);
	
	AddItemAtIndex(HoverItem, GridIndex, OldHoverItem.bStackable, OldHoverItem.StackCount);
	InventoryStorage->UpdateGridSlots(HoverItem, GridIndex, OldHoverItem.bStackable, OldHoverItem.StackCount);
}

bool UInventoryComponent::Server_SwapSelectedWitItem_Validate(UInventoryItem* ItemOnGrid, int32 GridIndex)
{
	return true;
}

//#if UE_WITH_CHEAT_MANAGER
void UInventoryComponent::DebugPrintStorage() const
{
	if (!IsValid(InventoryStorage))
	{
		DebugPrint(TEXT("DebugPrintStorage: Inventory Storage is null"), FColor::Red);
		return;
	}

	TArray<UInventoryItem*> AllItems = InventoryList.GetAllItems();

	// For now, each item will have its own letter A-Z. It can be changed in future
	auto GetItemIndex = [&AllItems](const UInventoryItem* Item) -> TCHAR
	{
		return Item ? 'A' + AllItems.IndexOfByKey(Item) : '-';
	};
	checkf(AllItems.Num() < ('Z' - 'A'), TEXT("Probably we need more robust identification here"));
	FStringBuilderBase Output;
	Output.Append(TEXT("Items:\n"));
	for (int32 Index = 0; Index < AllItems.Num(); Index++)
	{
		Output.Appendf(TEXT("%c: %s\n"), 'A' + Index, *GetInventoryItemId(AllItems[Index]));
	}
	Output.AppendChar('\n');

	Output.Append(TEXT("Storage:\n"));
	InventoryStorage->DebugPrintStorage(Output, GetItemIndex);

	Output += TEXT("Equipment slots:\n");
	for (const auto& EquipSlot : EquipmentSlots.GetAllItems())
	{
		Output.Appendf(TEXT("Slot %d (%s): (%c) %s\n"), static_cast<int32>(EquipSlot.GetSlotId()),
			*EquipSlot.GetEquipmentTypeTag().ToString(),
			GetItemIndex(EquipSlot.GetInventoryItem().Get()),
			*GetInventoryItemId(EquipSlot.GetInventoryItem().Get()));
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), Output.ToString());
}
//#endif//UE_WITH_CHEAT_MANAGER

void UInventoryComponent::RemoveItemFromStorage(UInventoryItem* Item, int32 GridIndex)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Removing item [%s] from storage index %d"), *GetInventoryItemId(Item), GridIndex)
	checkf(GetOwner()->HasAuthority(), TEXT("This method should be run only on server"));

	if (!IsValid(Item))
		return;
	
	check(InventoryStorage);
	InventoryStorage->RemoveItemFromGrid(Item, GridIndex);
}

void UInventoryComponent::Server_SelectItem_Implementation(UInventoryItem* Item, int32 GridIndex, int32 PrevIndex)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s] (index %d)"), *GetInventoryItemId(Item), GridIndex)

	if (!IsValid(Item))
	{
		UE_LOG(LogInventory, Error, TEXT("Server: Trying to select NULL item."))
		return;
	}
	
	if (!HasHoverItem())
	{
		HoverItemProxy = FHoverItemProxy();
	}
	HoverItemProxy->InventoryItem = Item;
	HoverItemProxy->bStackable = Item->IsStackable();
	
	HoverItemProxy->StackCount = HoverItemProxy->bStackable ? GetInventoryStorage()->GetItemStackCount(Item, GridIndex) : 0;
	if (PrevIndex != INDEX_NONE)
	{
		HoverItemProxy->PreviousIndex = PrevIndex;
	}

	RemoveItemFromStorage(Item, GridIndex);
	NotifyHoverItemUpdated();
}

bool UInventoryComponent::Server_SelectItem_Validate(UInventoryItem* Item, int32 GridIndex, int32 PrevIndex)
{
	return true;
}

void UInventoryComponent::Server_SelectItemInSlot_Implementation(EInventoryEquipmentSlot SlotId)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Selecting item in slot %d"), static_cast<int32>(SlotId))

	const FInventoryEquipmentSlot* EquipSlot = GetEquipmentSlot(SlotId);
	if (!EquipSlot)
	{
		UE_LOG(LogInventory, Error, TEXT("Server: Trying to select item in invalid slot %d"), static_cast<int32>(SlotId))
		return;
	}

	if ( HasHoverItem() && !HoverItemProxy->bStackable)
	{
		UE_LOG(LogInventory, Error, TEXT("Server: Cannot equip stackable item [%s]"), *GetInventoryItemId(HoverItemProxy->InventoryItem.Get()))
		return;
	}

	UInventoryItem* ItemToUnequip = EquipSlot->GetInventoryItem().Get();
	UInventoryItem* ItemToEquip = HasHoverItem() ? HoverItemProxy->InventoryItem.Get() : nullptr;

	Server_EquipItem(ItemToEquip, ItemToUnequip, SlotId);

	if (ItemToUnequip)
	{
		if (!HasHoverItem())
		{
			HoverItemProxy = FHoverItemProxy();
		}
		HoverItemProxy->InventoryItem = ItemToUnequip;
		NotifyHoverItemUpdated();
	}
	else
	{
		ClearSelectedItem();
	}
}

bool UInventoryComponent::Server_SelectItemInSlot_Validate(EInventoryEquipmentSlot SlotId)
{
	return true;
}

void UInventoryComponent::Server_PutSelectedItemToStorage_Implementation()
{
	if (!HasHoverItem())
		return;
	
	FInventorySlotAvailabilityResult Result;
	ensure(GetInventoryStorage()->HasRoomForItem(Result, HoverItemProxy->InventoryItem->GetItemManifest(), HoverItemProxy->StackCount));
	Result.Item = HoverItemProxy->InventoryItem;

	OnStackChanged.Broadcast(Result);

	ClearSelectedItem();
}

bool UInventoryComponent::Server_PutSelectedItemToStorage_Validate()
{
	return true;
}

void UInventoryComponent::AddItemAtIndex(UInventoryItem* Item, int32 Index, bool bStackable, int32 StackCount)
{
	const FInventorySlotAvailabilityResult Result = FInventorySlotAvailabilityResult::Make(Item, Index, bStackable, StackCount);
	OnStackChanged.Broadcast(Result);
}

void UInventoryComponent::Server_PutSelectedItemToStorageAtIndex_Implementation(const int32 TargetIndex)
{
	LOG_NETFUNCTIONCALL

	UInventoryItem* Item = HasHoverItem()? HoverItemProxy->InventoryItem.Get() : nullptr;
	if (!IsValid(Item))
	{
		UE_LOG(LogInventory, Error, TEXT("Server: Trying to put NULL item to storage."))
		return;
	}

	AddItemAtIndex(Item, TargetIndex, HoverItemProxy->bStackable, HoverItemProxy->StackCount);
	
	check(InventoryStorage);
	InventoryStorage->UpdateGridSlots(Item, TargetIndex, HoverItemProxy->bStackable, HoverItemProxy->StackCount);
	
	ClearSelectedItem();
}

bool UInventoryComponent::Server_PutSelectedItemToStorageAtIndex_Validate(const int32 TargetIndex)
{
	return true;
}

void UInventoryComponent::Server_DropSelectedItemOff_Implementation()
{
	LOG_NETFUNCTIONCALL
	
	UInventoryItem* Item = HasHoverItem()? HoverItemProxy->InventoryItem.Get() : nullptr;
	if (!IsValid(Item))
	{
		UE_LOG(LogInventory, Error, TEXT("Server: Trying to drop NULL item."))
		return;
	}
	
	Server_DropItem(HoverItemProxy->InventoryItem.Get(), HoverItemProxy->StackCount);

	ClearSelectedItem();
}

bool UInventoryComponent::Server_DropSelectedItemOff_Validate()
{
	return true;
}

void UInventoryComponent::Server_SplitStackToHoverItem_Implementation(UInventoryItem* Item, int32 GridIndex, int32 SplitAmount)
{
	LOG_NETFUNCTIONCALL

	if (!IsValid(Item) || !Item->IsStackable())
		return;
	
	int32 StackCount =  GetInventoryStorage()->GetItemStackCount(Item, GridIndex);
	if (StackCount < 2)
	{
		UE_LOG(LogInventory, Error, TEXT("Cannot split stack with 1 item: [%s] index: %d"), *GetInventoryItemId(Item), GridIndex)
		ClearSelectedItem(); // just in case, to prevent next usage
		return;
	}
	
	if (!HasHoverItem())
	{
		HoverItemProxy.Emplace(Item, GridIndex, StackCount, Item->IsStackable());
	}

	SplitAmount = FMath::Min(SplitAmount, StackCount - 1);

	HoverItemProxy->StackCount = SplitAmount;
	GetInventoryStorage()->SetItemStackCount(Item, GridIndex, StackCount - SplitAmount);
	NotifyHoverItemUpdated();
}

bool UInventoryComponent::Server_SplitStackToHoverItem_Validate(UInventoryItem* Item, int32 GridIndex, int32 SplitAmount)
{
	return true;
}

void UInventoryComponent::NotifyHoverItemUpdated()
{
	if (OwningPlayerController->IsLocalController())
	{
		OnHoverItemUpdated.Broadcast(HoverItemProxy->InventoryItem.Get(), HoverItemProxy->bStackable, HoverItemProxy->StackCount, HoverItemProxy->PreviousIndex);
	}
	else if (OwningPlayerController->HasAuthority())
	{
		Client_ReceivedHoverItemUpdated(HoverItemProxy->InventoryItem.Get(), HoverItemProxy->bStackable, HoverItemProxy->StackCount, HoverItemProxy->PreviousIndex);
	}
}

void UInventoryComponent::Client_ReceivedHoverItemUpdated_Implementation(UInventoryItem* InventoryItem, bool bStackable, int32 StackCount, int32 PreviousIndex)
{
	LOG_NETFUNCTIONCALL
	
	HoverItemProxy.Emplace(InventoryItem, PreviousIndex, StackCount, bStackable);
	NotifyHoverItemUpdated();
}
