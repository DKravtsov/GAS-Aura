// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const struct FAuraAttributeInfo&, Info);

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FAttributeInfoSignature AttributeInfoDelegate;
    
    UPROPERTY(BlueprintAssignable, Category="GAS|Player Stats")
    FOnIntStatChangedSignature OnAttributePointsChanged;
    
    UPROPERTY(BlueprintAssignable, Category="GAS|Player Stats")
    FOnIntStatChangedSignature OnSpellPointsChanged;
    
protected:

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<class UAttributeInfo> AttributeInfo;

public:

    UAttributeMenuWidgetController();

    void BroadcastInitialValues() override;

    void BindCallbacks() override;

protected:

    void BroadcastAttributeInfo(const FGameplayAttribute& Attr);
};
