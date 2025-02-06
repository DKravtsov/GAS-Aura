// Copyright 4sandwiches


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    //OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
    //OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
    //OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
    //OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

    OnAttributeChanged.Broadcast(AuraAttributeSet->GetHealthAttribute(), AuraAttributeSet->GetHealth());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetMaxHealthAttribute(), AuraAttributeSet->GetMaxHealth());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetManaAttribute(), AuraAttributeSet->GetMana());
    OnAttributeChanged.Broadcast(AuraAttributeSet->GetMaxManaAttribute(), AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacks()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
    check(AbilitySystemComponent);
    //AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
    //    .AddUObject(this, &UOverlayWidgetController::HealthChanged);
    //AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
    //    .AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
        .AddUObject(this, &UOverlayWidgetController::AttributeChanged);
}

//void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data)
//{
//    OnHealthChanged.Broadcast(Data.NewValue);
//}
//
//void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data)
//{
//    OnMaxHealthChanged.Broadcast(Data.NewValue);
//}

void UOverlayWidgetController::AttributeChanged(const FOnAttributeChangeData& Data)
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue);
}
