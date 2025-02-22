// Copyright 4sandwiches


#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Logs.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfoDataAsset.h"
#include "Characters/AuraPlayerCharacter.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

UAuraWidgetController::UAuraWidgetController()
{
}

void UAuraWidgetController::InitWidgetController(APlayerController* PC)
{
    if (!IsValid(PC))
    {
        return;
    }

    AuraPlayerController = CastChecked<AAuraPlayerController>(PC);
    AuraPlayerState = PC->GetPlayerState<AAuraPlayerState>();
    checkf(AuraPlayerState != nullptr, TEXT("PlayerState must be inherited from AuraPlayerState class"));

    AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AuraPlayerState));
    AuraAttributeSet = CastChecked<UAuraAttributeSet>(AuraAbilitySystemComponent->GetAttributeSet(UAuraAttributeSet::StaticClass()));
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacks()
{
}

void UAuraWidgetController::BroadcastAbilityInfo() const
{
    if (!AuraAbilitySystemComponent->AreStartupAbilitiesGiven())
    {
        return;
    }

    AuraAbilitySystemComponent->ForEachAbility(FForEachAbilityDelegate::CreateLambda([this](const FGameplayAbilitySpec& AbilitySpec)
        {
            const auto Tag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);
            UE_LOG(LogMyGame, Warning, TEXT("== %s : %s =="), *AbilitySpec.Ability->GetClass()->GetName(), *Tag.ToString());
            auto Info = GetAbilityInfo()->FindAbilityInfoByTag(Tag, false);
            Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
            Info.StatusTag = UAuraAbilitySystemComponent::GetStatusTagFromSpec(AbilitySpec);
            Info.SpellLevel = AbilitySpec.Level;
            OnReceivedAbilityInfo.Broadcast(Info);
        }));
}
