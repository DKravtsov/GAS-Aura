// Copyright 4sandwiches


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfoDataAsset.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetHealthAttribute(), AuraAttributeSet->GetHealth());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetMaxHealthAttribute(), AuraAttributeSet->GetMaxHealth());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetManaAttribute(), AuraAttributeSet->GetMana());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetMaxManaAttribute(), AuraAttributeSet->GetMaxMana());

    if (bForceInitializeStartupAbilities)
    {
        BroadcastAbilityInfo();
    }

    OnXPPercentChanged.Broadcast(AuraPlayerState->GetXP());
    OnLevelChanged.Broadcast(AuraPlayerState->GetPlayerLevel(), false);
}

void UOverlayWidgetController::BindCallbacks()
{
    check(AuraAbilitySystemComponent);

    AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);

	AuraAbilitySystemComponent->OnAbilityEquipped.AddUObject(this, &UOverlayWidgetController::AbilityEquipped);
    AuraAbilitySystemComponent->OnEffectApplied.AddUObject(this, &UOverlayWidgetController::BroadcastMessagesByTags);
    if (AuraAbilitySystemComponent->AreStartupAbilitiesGiven())
    {
        bForceInitializeStartupAbilities = true;
    }
    else
    {
        AuraAbilitySystemComponent->OnAbilitiesGiven.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
    }

    AuraPlayerState->OnXPChanged.AddLambda([this](const int32 NewXP)
        {
            const float PercentXP = AuraPlayerState->LevelUpData->GetLevelPercent(NewXP);
            OnXPPercentChanged.Broadcast(PercentXP); 
        });
    AuraPlayerState->OnLevelChanged.AddLambda([this](const int32 NewLevel, const bool bLevelUp)
        {
            OnLevelChanged.Broadcast(NewLevel, bLevelUp); 
        });
}

void UOverlayWidgetController::AttributeChanged(const FOnAttributeChangeData& Data) const
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue);
}

void UOverlayWidgetController::BroadcastMessagesByTags(const FGameplayTagContainer& AssetTags) const
{
    static FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));

    for (const FGameplayTag& Tag : AssetTags)
    {
        // filter all messages that start from "Message.XXX"
        if (!Tag.MatchesTag(MessageTag))
            continue;

        if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable.Get(), Tag))
        {
            OnReceiveUIMessage.Broadcast(*Row);
        }
    }
}
