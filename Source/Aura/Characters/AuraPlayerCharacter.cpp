// Copyright 4sandwiches


#include "Characters/AuraPlayerCharacter.h"

#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Player/AuraPlayerController.h"
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
    GrantStartupAbilities();
    InitOverlay();
}

void AAuraPlayerCharacter::InitAbilitySystemComponent()
{
    auto PS = GetPlayerStateChecked<AAuraPlayerState>();

    AbilitySystemComponent = PS->GetAbilitySystemComponent();
    AttributeSet = PS->GetAttributeSet();

    check(AbilitySystemComponent);

    AbilitySystemComponent->InitAbilityActorInfo(PS, this);

    if (GetLocalRole() == ROLE_Authority)
    {
        InitializeDefaultAttributes();
    }
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

    if (bDead || GetPlayerState() == nullptr)
        return;
    
    // init for the client
    InitAbilitySystemComponent();
    InitOverlay();
}

int32 AAuraPlayerCharacter::BP_GetCharacterLevel_Implementation() const
{
    if (const auto PS = GetPlayerState<AAuraPlayerState>())
    {
        CachedPlayerLevel = PS->GetPlayerLevel();
    }
    return CachedPlayerLevel;
}

void AAuraPlayerCharacter::AddXP_Implementation(int32 Amount)
{
    auto PS = GetPlayerStateChecked<AAuraPlayerState>();
    PS->AddXP(Amount);
}

EAuraCharacterClass AAuraPlayerCharacter::GetCharacterClass_Implementation() const
{
    return EAuraCharacterClass::Elementalist;
}

void AAuraPlayerCharacter::NotifyLevelUp_Implementation()
{
    ApplyGameplayEffectToSelf(LevelUpEffect, GetCharacterLevel());
}

void AAuraPlayerCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
    if (AAuraPlayerController* PC = GetController<AAuraPlayerController>())
    {
        PC->ShowMagicCircle(DecalMaterial);
        PC->SetShowMouseCursor(false);
    }
}

void AAuraPlayerCharacter::HideMagicCircle_Implementation()
{
    if (AAuraPlayerController* PC = GetController<AAuraPlayerController>())
    {
        PC->HideMagicCircle();
        PC->SetShowMouseCursor(true);
    }
}

void AAuraPlayerCharacter::SaveProgress_Implementation(FName CheckpointTag)
{
    if (AAuraGameModeBase* AuraGameMode = GetWorld()->GetAuthGameMode<AAuraGameModeBase>())
    {
        if (ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData())
        {
            SaveData->PlayerStartTag = CheckpointTag;

            AuraGameMode->SaveInGameProgressData(SaveData);
        }
    }
}
