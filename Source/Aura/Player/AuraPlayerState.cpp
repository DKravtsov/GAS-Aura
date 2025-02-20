// Copyright 4sandwiches


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
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
    // #todo: broadcast old and new values OR num levels up
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
}

void AAuraPlayerState::AddAttributePoints(int32 Amount)
{
}

void AAuraPlayerState::SetSpellPoints(int32 NewValue)
{
}

void AAuraPlayerState::AddSpellPoints(int32 Amount)
{
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

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAuraPlayerState, Level);
    DOREPLIFETIME(AAuraPlayerState, CurrentXP);
}

#pragma endregion 