// Copyright 4sandwiches


#include "Characters/AuraPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/HUD/AuraHUD.h"

AAuraPlayerCharacter::AAuraPlayerCharacter()
{

    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
    CameraBoom->TargetArmLength = 800.f;
    CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
    CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AAuraPlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // init for the server
    InitAbilitySystemComponent();
    InitOverlay();
}

void AAuraPlayerCharacter::InitAbilitySystemComponent()
{
    auto PS = GetPlayerStateChecked<AAuraPlayerState>();

    AbilitySystemComponent = PS->GetAbilitySystemComponent();
    AttributeSet = PS->GetAttributeSet();

    check(AbilitySystemComponent);

    AbilitySystemComponent->InitAbilityActorInfo(PS, this);

}

void AAuraPlayerCharacter::InitOverlay()
{
    if (auto PC = Cast<APlayerController>(GetController()))
    {
        if (auto HUD = PC->GetHUD<AAuraHUD>())
        {
            HUD->InitOverlay();
        }
    }
}

void AAuraPlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // init for the client
    InitAbilitySystemComponent();
    InitOverlay();
}
