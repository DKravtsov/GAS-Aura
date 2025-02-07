// Copyright 4sandwiches


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    OnAttributeChanged.Broadcast(AuraAttributeSet->GetHealthAttribute(), AuraAttributeSet->GetHealth());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetMaxHealthAttribute(), AuraAttributeSet->GetMaxHealth());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetManaAttribute(), AuraAttributeSet->GetMana());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetMaxManaAttribute(), AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacks()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
    check(AbilitySystemComponent);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);

    auto AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
    AuraAbilitySystemComponent->OnEffectApplied.AddUObject(this, &UOverlayWidgetController::BroadcastMessagesByTags);
}

void UOverlayWidgetController::AttributeChanged(const FOnAttributeChangeData& Data)
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue);
}

void UOverlayWidgetController::BroadcastMessagesByTags(const FGameplayTagContainer& AssetTags)
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
