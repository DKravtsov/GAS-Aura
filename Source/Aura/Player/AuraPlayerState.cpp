// Copyright 4sandwiches


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
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
    Level = NewLevel;
    OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::SetXP(int32 NewValue)
{
    if (NewValue > CurrentXP) // just in case
    {
        CurrentXP = NewValue;
        OnXPChanged.Broadcast(CurrentXP);
    }
}

void AAuraPlayerState::AddXP(int32 Value)
{
    SetXP(CurrentXP + Value);
}

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
