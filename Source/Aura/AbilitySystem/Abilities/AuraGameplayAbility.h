// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AuraGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EAuraAbilityActivationPolicy : uint8
{
    OnTriggered,
    OnGiven,
};


/**
 *
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditDefaultsOnly, Category = AuraAbility, meta = (Categories = "InputTag", EditCondition = "ActivationPolicy==EAuraAbilityActivationPolicy::OnTriggered"))
    FGameplayTag StartupInputTag;

protected:
    
    UPROPERTY(EditDefaultsOnly, Category = AuraAbility)
    EAuraAbilityActivationPolicy ActivationPolicy = EAuraAbilityActivationPolicy::OnTriggered;

public:

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION(BlueprintNativeEvent)
    void GetDescription(const int32 Level, FText& OutDesc, FText& OutNextLevelDesc) const;

    UFUNCTION(BlueprintPure)
    float GetManaCost(const int32 InLevel) const;

    UFUNCTION(BlueprintPure)
    float GetCooldown(const int32 InLevel) const;
    
protected:

    class UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;
};
