// Copyright 4sandwiches


#include "Player/InventoryPlayerControllerComponent.h"
#include "EnhancedInputComponent.h"
#include "Interfaces/InventoryHighlightableInterface.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Widgets/HUD/InventoryHUDWidget.h"

UInventoryPlayerControllerComponent::UInventoryPlayerControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryPlayerControllerComponent::PerformInteractionTraceFromCamera()
{
	// TODO: need to implement
	unimplemented();
}

void UInventoryPlayerControllerComponent::PerformInteractionTraceUnderMouse()
{
	const APlayerController* PC = GetOwningPlayerController();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(InteractionTraceChannel, false, CursorHit);
	UpdateInteractionTraceResult(CursorHit.bBlockingHit ? CursorHit.GetActor() : nullptr);
}

void UInventoryPlayerControllerComponent::UpdateInteractionTraceResult(AActor* InteractableActor)
{
	const AActor* LastActor = CurrentInteractableActor.Get();

	if (LastActor != InteractableActor)
	{
		if (LastActor)
		{
			const TArray<UActorComponent*> Highlightables = LastActor->GetComponentsByInterface(UInventoryHighlightableInterface::StaticClass());
			for (UActorComponent* Highlightable : Highlightables)
			{
				IInventoryHighlightableInterface::Execute_UnHighlight(Highlightable);
			}
		}
		
		if (!IsValid(InteractableActor))
		{
			if (IsValid(HUDWidget))
			{
				HUDWidget->HidePickupMessage();
			}
		}
		else
		{
			if (UInventoryItemComponent* ItemComponent = InteractableActor->FindComponentByClass<UInventoryItemComponent>(); IsValid(ItemComponent))
			{
				if (IsValid(HUDWidget))
				{
					HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
				}
			}
			const TArray<UActorComponent*> Highlightables = InteractableActor->GetComponentsByInterface(UInventoryHighlightableInterface::StaticClass());
			for (UActorComponent* Highlightable : Highlightables)
			{
				IInventoryHighlightableInterface::Execute_Highlight(Highlightable);
			}
		}

		CurrentInteractableActor = InteractableActor;
	}
}

void UInventoryPlayerControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryComponent = GetOwner()->GetComponentByClass<UInventoryComponent>();

	// TODO: need to take into account that add the widget to the viewport can break the target's UI rules
	// Consider adding callback or something like this
	CreateHUDWidget();
}

void UInventoryPlayerControllerComponent::SetupInputComponent(UEnhancedInputComponent* InputComponent)
{
	check(InputComponent);
	if (IsValid(PrimaryInteractionAction))
	{
		InputComponent->BindAction(PrimaryInteractionAction, ETriggerEvent::Started, this, &UInventoryPlayerControllerComponent::PrimaryInteract);
		InputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &UInventoryPlayerControllerComponent::ToggleInventory);
	}
}

void UInventoryPlayerControllerComponent::PrimaryInteract()
{
	UE_LOG(LogTemp, Log, TEXT("UInventoryPlayerControllerComponent::PrimaryInteract"));
}

void UInventoryPlayerControllerComponent::CreateHUDWidget()
{
	if (const auto PC = GetOwningPlayerController())
	{
		if (!PC->IsLocalController())
			return;

		HUDWidget = CreateWidget<UInventoryHUDWidget>(PC, HUDWidgetClass);
		if (IsValid(HUDWidget))
		{
			HUDWidget->AddToViewport();
		}
	}
}

void UInventoryPlayerControllerComponent::ToggleInventory()
{
	if (InventoryComponent.IsValid())
	{
		InventoryComponent->ToggleInventoryMenu();
	}
}

