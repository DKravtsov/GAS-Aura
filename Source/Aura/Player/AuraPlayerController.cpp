// Copyright 4sandwiches


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
    SetReplicates(true);
}

void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();

    checkf(InputContext, TEXT("Input Context isn't set"));

    if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        InputSubsystem->AddMappingContext(InputContext, 0);
    }

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputValue)
{
    const FVector2D InputVector = InputValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
    const FRotationMatrix RotationMatrix(YawRotation);

    const FVector Forward = RotationMatrix.GetUnitAxis(EAxis::X);
    const FVector Right = RotationMatrix.GetUnitAxis(EAxis::Y);

    if (auto ControlledPawn = GetPawn())
    {
        ControlledPawn->AddMovementInput(Forward, InputVector.Y);
        ControlledPawn->AddMovementInput(Right, InputVector.X);
    }
}
