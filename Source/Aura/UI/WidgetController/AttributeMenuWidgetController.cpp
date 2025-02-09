// Copyright 4sandwiches


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystemComponent.h"

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
}
