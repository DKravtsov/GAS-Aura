// Copyright 4sandwiches


#include "InventoryManagement/Components/InventoryComponent.h"

#include "Items/Components/InventoryItemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "Player/InventoryPlayerControllerComponent.h"
#include "Items/InventoryItem.h"

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
		// Add stacks to an item that already exists in the inventory.Only need to update the stack count
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else
	{
		// This item doesn't exist in the inventory. Need to create one and update all related stuff
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 1);
	}
}

void UInventoryComponent::Server_AddNewItem_Implementation(UInventoryItemComponent* ItemComponent, int32 StackCount)
{
	auto NewItem = InventoryList.AddItem(ItemComponent);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}

	// TODO: tell the item component to destroy its owning actor
}

bool UInventoryComponent::Server_AddNewItem_Validate(UInventoryItemComponent* ItemComponent, int32 StackCount)
{
	return true;
}

void UInventoryComponent::Server_AddStacksToItem_Implementation(UInventoryItemComponent* ItemComponent,	int32 StackCount, int32 Remainder)
{
}

bool UInventoryComponent::Server_AddStacksToItem_Validate(UInventoryItemComponent* ItemComponent, int32 StackCount,	int32 Remainder)
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
}

void UInventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu))
		return;
	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
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
		OwningPlayerController->SetInputMode(InputMode);
	}
}
