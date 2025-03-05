// Copyright 4sandwiches


#include "Characters/AuraEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widgets/AuraUserWidget.h"
#include "Game/AuraGameModeBase.h"
#include "AuraGameplayTags.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraEnemyCharacter::AAuraEnemyCharacter()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bUseControllerDesiredRotation = true;

    RewardXP = -1;
}

void AAuraEnemyCharacter::HighlightActor_Implementation()
{
    if (USkeletalMeshComponent* MyMesh = GetMesh())
    {
        MyMesh->SetRenderCustomDepth(true);
        MyMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_RED);
    }
    if (Weapon)
    {
        Weapon->SetRenderCustomDepth(true);
        Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_RED);
    }
}

void AAuraEnemyCharacter::UnhighlightActor_Implementation()
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
        Widget->SetWidgetController(this);
    }

    if (UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
    {

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

        OnHealthAttributesChanged.Broadcast(AuraAttributeSet->GetHealthAttribute(), AuraAttributeSet->GetHealth());
        OnHealthAttributesChanged.Broadcast(AuraAttributeSet->GetMaxHealthAttribute(), AuraAttributeSet->GetMaxHealth());

        AbilitySystemComponent->RegisterGameplayTagEvent(AuraGameplayTags::Abilities_HitReact, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &AAuraEnemyCharacter::HitReactTagChanged);
    }
}

EAuraCharacterClass AAuraEnemyCharacter::GetCharacterClass_Implementation() const
{
    return CharacterClass;
}

int32 AAuraEnemyCharacter::GetRewardXP_Implementation() const
{
    return RewardXP.GetValueAtLevel(GetCharacterLevel());
}

void AAuraEnemyCharacter::Die(AActor* KillerActor, const FVector& DeathImpulse)
{
    SetLifeSpan(DeadBodyLifeSpan);
    if (IsValid(AuraAIController))
    {
        AuraAIController->GetBlackboardComponent()->SetValueAsBool("bDead", true);
    }
    SpawnLoot();
    Super::Die(KillerActor, DeathImpulse);
}

FAttackInfo AAuraEnemyCharacter::GetTaggedMontageByTag_Implementation(FGameplayTag MontageTag)
{
    for (const auto& TaggedMontage : AttackMontages)
    {
        if (TaggedMontage.MontageTag == MontageTag)
        {
            return TaggedMontage;
        }
    }
    return FAttackInfo();
}

void AAuraEnemyCharacter::HitReactTagChanged(const FGameplayTag Tag, int32 Count)
{
    bHitReacting = Count > 0;
    GetCharacterMovement()->StopMovementImmediately();
    if (AuraAIController && AuraAIController->GetBlackboardComponent())
    {
        AuraAIController->GetBlackboardComponent()->SetValueAsBool("bHitReacting", bHitReacting);
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

        if (BehaviorTree == nullptr)
        {
            UWorld* World = GetWorld();
            AAuraGameModeBase* GM = World ? World->GetAuthGameMode<AAuraGameModeBase>() : nullptr;
            if (GM != nullptr && GM->CharacterClassInfo != nullptr)
            {
                BehaviorTree = GM->CharacterClassInfo->GetClassDefaultInfo(CharacterClass).ClassBehaviorTree;
            }
        }
       
        AuraAIController = Cast<AAuraAIController>(NewController);

        AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
        AuraAIController->RunBehaviorTree(BehaviorTree);
        AuraAIController->GetBlackboardComponent()->SetValueAsBool("bHitReacting", false);
        AuraAIController->GetBlackboardComponent()->SetValueAsBool("bRangeAttacker", CharacterClass != EAuraCharacterClass::Warrior);
    }

}

void AAuraEnemyCharacter::InitializeDefaultAttributes()
{
    UWorld* World = GetWorld();
    auto GM = World ? World->GetAuthGameMode<AAuraGameModeBase>() : nullptr;

    if (GM != nullptr && GM->CharacterClassInfo != nullptr)
    {
        // set default attributes if empty

        const FCharacterClassDefaultInfo DefaultInfo = GM->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

        DefaultPrimaryAttributes = DefaultPrimaryAttributes? DefaultPrimaryAttributes : DefaultInfo.PrimaryAttributes;
        DefaultSecondaryAttributes = DefaultSecondaryAttributes ? DefaultSecondaryAttributes : GM->CharacterClassInfo->SecondaryAttributes;
        DefaultVitalAttributes = DefaultVitalAttributes ? DefaultVitalAttributes : GM->CharacterClassInfo->VitalAttributes;

        if (RewardXP.IsStatic() && RewardXP.Value < 0)
        {
            RewardXP = DefaultInfo.RewardXP;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot initialize default attributes"))
    }

    // apply all effects
    Super::InitializeDefaultAttributes();
}

void AAuraEnemyCharacter::GrantStartupAbilities()
{
    UWorld* World = GetWorld();
    auto GM = World ? World->GetAuthGameMode<AAuraGameModeBase>() : nullptr;

    if (GM != nullptr && GM->CharacterClassInfo != nullptr)
    {
        // Append class abilities to the character startup abilities array
        // (this works only on the server)

        StartupAbilities.Append(GM->CharacterClassInfo->Abilities);
        const FCharacterClassDefaultInfo& Info = GM->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
        StartupAbilities.Append(Info.Abilities);
    }

    Super::GrantStartupAbilities();
}

void AAuraEnemyCharacter::UpdateBlackboardMinionCount() const
{
    if (HasAuthority() && IsValid(AuraAIController))
    {
        check(AuraAIController == Controller);
        AuraAIController->GetBlackboardComponent()->SetValueAsInt("MinionCount", MinionCount);
    }
}

void AAuraEnemyCharacter::AddedMinion(APawn* NewMinion)
{
    UpdateBlackboardMinionCount();
}

void AAuraEnemyCharacter::RemovedMinion(APawn* Minion)
{
    UpdateBlackboardMinionCount();
}
