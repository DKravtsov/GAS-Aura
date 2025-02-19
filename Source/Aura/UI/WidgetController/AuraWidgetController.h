// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedSignature, const struct FGameplayAttribute&, Attribute, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatStatChangedSignature, const float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntStatChangedSignature, const int32, NewValue);

/**
 *
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
    GENERATED_BODY()

protected:

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<class APlayerController> PlayerController;

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<class APlayerState> PlayerState;

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<class UAttributeSet> AttributeSet;

public:

    UAuraWidgetController();

    UFUNCTION(BlueprintCallable)
    void InitWidgetController(APlayerController* PC);

    UFUNCTION(BlueprintCallable)
    virtual void BroadcastInitialValues();

    virtual void BindCallbacks();
};
