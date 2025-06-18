// Copyright 4sandwiches


#include "Player/InventoryPlayerControllerComponent.h"
#include "EnhancedInputComponent.h"

UInventoryPlayerControllerComponent::UInventoryPlayerControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryPlayerControllerComponent::BeginPlay()
{
	Super::BeginPlay();

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
