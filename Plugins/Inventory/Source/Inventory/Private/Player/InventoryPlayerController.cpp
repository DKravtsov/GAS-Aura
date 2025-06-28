// Copyright 4sandwiches


#include "Player/InventoryPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Player/InventoryPlayerControllerComponent.h"

AInventoryPlayerController::AInventoryPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryComp = CreateDefaultSubobject<UInventoryPlayerControllerComponent>("InventoryComp");
}

void AInventoryPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (IsValid(InputSubsystem))
	{
		InputSubsystem->AddMappingContext(DefaultInputMappingContext , 0);
	}
}
