// Copyright 4sandwiches


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

UAttributeMenuWidgetController::UAttributeMenuWidgetController()
{
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    checkf(AttributeInfo != nullptr, TEXT("AttributeInfo is not specified in %s"), *GetNameSafe(this));

    TArray<FGameplayAttribute> AllAttributes;
    AuraAbilitySystemComponent->GetAllAttributes(AllAttributes);

    for (const auto& Attr : AllAttributes)
    {
        BroadcastAttributeInfo(Attr);
    }
    
    OnAttributePointsChanged.Broadcast(AuraPlayerState->GetAttributePoints());
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayAttribute& Attr) const
{
    FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfo(Attr);
    Info.Value = Attr.GetNumericValue(AuraAttributeSet);
    AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BindCallbacks()
{

    TArray<FGameplayAttribute> AllAttributes;
    AuraAbilitySystemComponent->GetAllAttributes(AllAttributes);

    for (const auto& Attr : AllAttributes)
    {
        AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attr)
            .AddLambda([this](const FOnAttributeChangeData& Data)
                {
                    BroadcastAttributeInfo(Data.Attribute);
                });
    }

    AuraPlayerState->OnAttributePointsChanged.AddLambda([this](const int32 AP)
        {
            OnAttributePointsChanged.Broadcast(AP);
        });
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag, const int32 Points)
{
    if (AuraPlayerState)
    {
        AuraPlayerState->UpgradeAttribute(AttributeTag, Points);
    }
}
