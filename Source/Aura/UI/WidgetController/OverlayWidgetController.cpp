// Copyright 4sandwiches


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    const UAuraAttributeSet* AuraAtributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    OnHealthChanged.Broadcast(AuraAtributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(AuraAtributeSet->GetMaxHealth());
    OnManaChanged.Broadcast(AuraAtributeSet->GetMana());
    OnMaxManaChanged.Broadcast(AuraAtributeSet->GetMaxMana());

    OnAttributeChanged.Broadcast(AuraAtributeSet->GetHealthAttribute(), AuraAtributeSet->GetHealth());
    OnAttributeChanged.Broadcast(AuraAtributeSet->GetMaxHealthAttribute(), AuraAtributeSet->GetMaxHealth());
    OnAttributeChanged.Broadcast(AuraAtributeSet->GetManaAttribute(), AuraAtributeSet->GetMana());
    OnAttributeChanged.Broadcast(AuraAtributeSet->GetMaxManaAttribute(), AuraAtributeSet->GetMaxMana());
}
