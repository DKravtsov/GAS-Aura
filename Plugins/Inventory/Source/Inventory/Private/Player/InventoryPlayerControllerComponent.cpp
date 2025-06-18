// Copyright 4sandwiches


#include "Player/InventoryPlayerControllerComponent.h"
#include "EnhancedInputComponent.h"
#include "Widgets/HUD/InventoryHUDWidget.h"

UInventoryPlayerControllerComponent::UInventoryPlayerControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryPlayerControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO: need to take into account that add the widget to the viewport can break the target's UI rules
	// Consider adding callback or something like this
	CreateHUDWidget();
}

void UInventoryPlayerControllerComponent::SetupInputComponent(UEnhancedInputComponent* InputComponent)
{
	check(InputComponent);
	if (IsValid(PrimaryInteractionAction))
	{
		InputComponent->BindAction(PrimaryInteractionAction, ETriggerEvent::Triggered, this, &UInventoryPlayerControllerComponent::PrimaryInteract);
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
