// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature, float, NewMana);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature, float, NewMaxMana);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedSignature, struct FGameplayAttribute, Attribute, float, NewValue);



/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
    GENERATED_BODY()

public:

    //UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    //FOnHealthChangedSignature OnHealthChanged;

    //UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    //FOnMaxHealthChangedSignature OnMaxHealthChanged;

    //UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    //FOnManaChangedSignature OnManaChanged;

    //UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    //FOnMaxManaChangedSignature OnMaxManaChanged;

    UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    FOnAttributeChangedSignature OnAttributeChanged;

public:

    void BroadcastInitialValues() override;

    void BindCallbacks() override;

protected:

    //void HealthChanged(const FOnAttributeChangeData& Data);
    //void MaxHealthChanged(const FOnAttributeChangeData& Data);

    void AttributeChanged(const FOnAttributeChangeData& Data);
};
