// Copyright 4sandwiches


#include "InventoryManagement/Components/InventoryComponent.h"

#include "Widgets/Inventory/Base/InventoryWidgetBase.h"
#include "Player/InventoryPlayerControllerComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
