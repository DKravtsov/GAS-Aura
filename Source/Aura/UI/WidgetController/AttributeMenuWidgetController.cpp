// Copyright 4sandwiches


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

UAttributeMenuWidgetController::UAttributeMenuWidgetController()
{
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    checkf(AttributeInfo != nullptr, TEXT("AttributeInfo is not specified in %s"), *GetNameSafe(this));

    TArray<FGameplayAttribute> AllAttributes;
    AbilitySystemComponent->GetAllAttributes(AllAttributes);

    for (const auto& Attr : AllAttributes)
    {
        BroadcastAttributeInfo(Attr);
    }
    
    if (AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState))
    {
        OnAttributePointsChanged.Broadcast(AuraPlayerState->GetAttributePoints());
        OnSpellPointsChanged.Broadcast(AuraPlayerState->GetSpellPoints());
    }
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayAttribute& Attr)
{
    FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfo(Attr);
    Info.Value = Attr.GetNumericValue(AttributeSet);
    AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BindCallbacks()
{

    TArray<FGameplayAttribute> AllAttributes;
    AbilitySystemComponent->GetAllAttributes(AllAttributes);

    for (const auto& Attr : AllAttributes)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attr)
            .AddLambda([this](const FOnAttributeChangeData& Data)
                {
                    BroadcastAttributeInfo(Data.Attribute);
                });
    }

    if (AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState))
    {
        AuraPlayerState->OnAttributePointsChanged.AddLambda([this](const int32 AP)
        {
            OnAttributePointsChanged.Broadcast(AP);
        });
        AuraPlayerState->OnSpellPointsChanged.AddLambda([this](const int32 SP)
        {
            OnSpellPointsChanged.Broadcast(SP); 
        });
    }
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag, const int32 Points)
{
    AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
    AuraPlayerState->UpgradeAttribute(AttributeTag, Points);
}
