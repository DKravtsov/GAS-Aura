// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedSignature, const struct FGameplayAttribute&, Attribute, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatStatChangedSignature, const float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntStatChangedSignature, const int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityInfoSignature, const struct FAuraAbilityInfo&, AbilityInfo);

/**
 *
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
    GENERATED_BODY()
public:
    
    UPROPERTY(BlueprintAssignable, Category="GAS|Abilities")
    FOnAbilityInfoSignature OnReceivedAbilityInfo;
    
protected:

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<class AAuraPlayerController> AuraPlayerController;

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<class AAuraPlayerState> AuraPlayerState;

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<class UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<const class UAuraAttributeSet> AuraAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Abilities")
    TObjectPtr<class UAbilityInfoDataAsset> AbilityInfo;

public:

    UAuraWidgetController();

    UFUNCTION(BlueprintCallable)
    void InitWidgetController(APlayerController* PC);

    UFUNCTION(BlueprintCallable)
    virtual void BroadcastInitialValues();

    virtual void BindCallbacks();

    class UAbilityInfoDataAsset* GetAbilityInfo() const { return AbilityInfo; }

protected:
    
    void BroadcastAbilityInfo() const;
};
