// Copyright 4sandwiches


#include "Characters/AuraEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widgets/AuraUserWidget.h"
#include "Game/AuraGameModeBase.h"

AAuraEnemyCharacter::AAuraEnemyCharacter()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemyCharacter::HighlightActor()
{
    if (USkeletalMeshComponent* MyMesh = GetMesh())
    {
        MyMesh->SetRenderCustomDepth(true);
        MyMesh->SetCustomDepthStencilValue(250);
    }
    if (Weapon)
    {
        Weapon->SetRenderCustomDepth(true);
        Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_RED);
    }
}

void AAuraEnemyCharacter::UnhighlightActor()
{
    if (USkeletalMeshComponent* MyMesh = GetMesh())
    {
        MyMesh->SetRenderCustomDepth(false);
    }
    if (Weapon)
    {
        Weapon->SetRenderCustomDepth(false);
    }
}

void AAuraEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (UAuraUserWidget* Widget = GetHealthOverlayWidget())
    {
        UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

        GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnHealthAttributesChanged.Broadcast(Data.Attribute, Data.NewValue);
            });
        GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetMaxHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnHealthAttributesChanged.Broadcast(Data.Attribute, Data.NewValue);
            });

        Widget->SetWidgetController(this);

        OnHealthAttributesChanged.Broadcast(AuraAttributeSet->GetHealthAttribute(), AuraAttributeSet->GetHealth());
        OnHealthAttributesChanged.Broadcast(AuraAttributeSet->GetMaxHealthAttribute(), AuraAttributeSet->GetMaxHealth());
    }
}

void AAuraEnemyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    AbilitySystemComponent->InitAbilityActorInfo(this, this);

    if (HasAuthority())
    {
        InitializeDefaultAttributes();

        GrantStartupAbilities();
    }
}

void AAuraEnemyCharacter::InitializeDefaultAttributes()
{
    UWorld* World = GetWorld();
    auto GM = World ? World->GetAuthGameMode<AAuraGameModeBase>() : nullptr;

    if (GM == nullptr || GM->CharacterClassInfo == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot initialize default attributes"))
        return;
    }

    const FCharacterClassDefaultInfo DefaultInfo = GM->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

    ApplyGameplayEffectToSelf(DefaultInfo.PrimaryAttributes, CharacterLevel);
    ApplyGameplayEffectToSelf(GM->CharacterClassInfo->SecondaryAttributes, CharacterLevel);
    ApplyGameplayEffectToSelf(GM->CharacterClassInfo->VitalAttributes, CharacterLevel);
}
