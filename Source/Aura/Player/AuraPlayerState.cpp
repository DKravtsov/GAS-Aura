// Copyright 4sandwiches


#include "Player/AuraPlayerState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Characters/CombatInterface.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
    NetUpdateFrequency = 100.f;

    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

class UAuraAbilitySystemComponent* AAuraPlayerState::GetAuraAbilitySystemComponent() const
{
    return Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
}

UAuraAbilitySystemComponent* AAuraPlayerState::GetAuraAbilitySystemComponentChecked() const
{
    return CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
}

class UAuraAttributeSet* AAuraPlayerState::GetAuraAttributeSetChecked() const
{
    return CastChecked<UAuraAttributeSet>(AttributeSet);
}

void AAuraPlayerState::SetPlayerLevel(int32 NewLevel)
{
    if (NewLevel <= Level)
        return;

    int32 NumLevelUps = NewLevel - Level;

    int32 AttributePointsReward = 0;
    int32 SpellPointsReward = 0;

    for (int32 Index = Level; Index < NewLevel; ++Index)
    {
        AttributePointsReward += LevelUpData->GetLevelUpInfo(Index).AttributePointReward;
        SpellPointsReward += LevelUpData->GetLevelUpInfo(Index).SpellPointReward;
    }

    AddAttributePoints(AttributePointsReward);
    AddSpellPoints(SpellPointsReward);

    Level = NewLevel;

    GetAuraAbilitySystemComponentChecked()->UpdateAbilityStatuses(Level);

    ICombatInterface::Execute_NotifyLevelUp(AbilitySystemComponent->GetAvatarActor());
    OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::AddPlayerLevel(int32 NumLevels)
{
    SetPlayerLevel(Level + NumLevels);
}

void AAuraPlayerState::SetXP(int32 NewValue)
{
    if (NewValue > CurrentXP) // just in case
    {
        int32 NewLevel = LevelUpData->FindLevelByXP(NewValue);
        CurrentXP = NewValue;
        OnXPChanged.Broadcast(CurrentXP);
        if (NewLevel > Level)
        {
            AddPlayerLevel(NewLevel - Level);
        }
    }
}

void AAuraPlayerState::AddXP(int32 Value)
{
    SetXP(CurrentXP + Value);
}

void AAuraPlayerState::SetAttributePoints(int32 NewValue)
{
    if (AttributePoints == NewValue)
        return;
    
    AttributePoints = NewValue;
    OnAttributePointsChanged.Broadcast(NewValue);
}

void AAuraPlayerState::AddAttributePoints(int32 Amount)
{
    SetAttributePoints(AttributePoints + Amount);
}

void AAuraPlayerState::SetSpellPoints(int32 NewValue)
{
    if (SpellPoints == NewValue)
        return;
    SpellPoints = NewValue;
    OnSpellPointsChanged.Broadcast(NewValue);
}

void AAuraPlayerState::AddSpellPoints(int32 Amount)
{
    SetSpellPoints(SpellPoints + Amount);
}

void AAuraPlayerState::UpgradeAttribute(const FGameplayTag& AttributeTag, const int32 Points)
{
    if (Points < 0 || (Points > 0 && AttributePoints >= Points))
    {
        ServerUpgradeAttribute(AttributeTag, Points);
    }
}

void AAuraPlayerState::UpgradeSpell(const FGameplayTag& AbilityTag)
{
    ServerUpgradeSpell(AbilityTag);
}

void AAuraPlayerState::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag, const int32 Points)
{
    FGameplayEventData Payload;
    Payload.EventTag = AttributeTag;
    Payload.EventMagnitude = Points;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AbilitySystemComponent->GetAvatarActor(), AttributeTag, Payload);

    AddAttributePoints(-Points);
}

bool AAuraPlayerState::ServerUpgradeAttribute_Validate(const FGameplayTag& AttributeTag, const int32 Points)
{
    return Points == 1 || Points == -1;
}


#pragma region Replication

void AAuraPlayerState::OnRep_Level(int32 OldValue)
{
    OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::OnRep_CurrentXP(int32 OldValue)
{
    OnXPChanged.Broadcast(CurrentXP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldValue)
{
    OnAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldValue)
{
    OnSpellPointsChanged.Broadcast(SpellPoints);
}

void AAuraPlayerState::ServerUpgradeSpell_Implementation(const FGameplayTag& AbilityTag)
{
    if (auto ASC = GetAuraAbilitySystemComponent())
    {
        if (ASC->UpgradeAbility(AbilityTag))
        {
            AddSpellPoints(-1);
        }
    }
}

bool AAuraPlayerState::ServerUpgradeSpell_Validate(const FGameplayTag& AbilityTag)
{
    return true;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAuraPlayerState, Level);
    DOREPLIFETIME(AAuraPlayerState, CurrentXP);
    DOREPLIFETIME(AAuraPlayerState, AttributePoints);
    DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

#pragma endregion 