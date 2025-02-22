// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

UENUM()
enum class EAbilityDamagePolicy : uint8
{
    NoDamage,
    CausesDamage,
};

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

    UPROPERTY(EditDefaultsOnly, Category = AuraAbility, meta = (Categories = "InputTag"))
    FGameplayTag StartupInputTag;

    UPROPERTY(EditDefaultsOnly, Category = AuraAbility)
    EAuraAbilityActivationPolicy ActivationPolicy = EAuraAbilityActivationPolicy::OnTriggered;

    UPROPERTY(EditDefaultsOnly, Category = AuraAbility)
    EAbilityDamagePolicy DamagePolicy = EAbilityDamagePolicy::NoDamage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage, meta = (EditCondition = "DamagePolicy==EAbilityDamagePolicy::CausesDamage", EditConditionHides))
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage, meta = (EditCondition = "DamagePolicy==EAbilityDamagePolicy::CausesDamage", EditConditionHides, Categories="Damage", ForceInlineRow))
    TMap<FGameplayTag, FScalableFloat> DamageTypes;

public:

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION(BlueprintCallable, Category = Caombat)
    void CauseDamageToActor(AActor* TargetActor);

    // Causes damage to the array of the actors, checking if they are not friendly.
    // Returns true if at least one damage was applied
    UFUNCTION(BlueprintCallable, Category = Caombat)
    bool CauseDamageToActors(const TArray<AActor*>& TargetActors);

    FText GetDescription(const int32 Level) const;
    FText GetNextLevelDescription(const int32 Level) const;

protected:

    void SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle);

};
