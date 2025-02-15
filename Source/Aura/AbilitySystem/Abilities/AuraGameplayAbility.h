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

/**
 *
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, Category = Input, meta = (Categories = "InputTag"))
    FGameplayTag StartupInputTag;

    UPROPERTY(EditDefaultsOnly, Category = Damage)
    EAbilityDamagePolicy DamagePolicy = EAbilityDamagePolicy::NoDamage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage, meta = (EditCondition = "DamagePolicy==EAbilityDamagePolicy::CausesDamage", EditConditionHides))
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage, meta = (EditCondition = "DamagePolicy==EAbilityDamagePolicy::CausesDamage", EditConditionHides, Categories="Damage", ForceInlineRow))
    TMap<FGameplayTag, FScalableFloat> DamageTypes;

public:

    UFUNCTION(BlueprintCallable, Category = Caombat)
    void CauseDamageToActor(AActor* TargetActor);

    // Causes damage to the array of the actors, checking if they are not friendly.
    // Returns true if at least one damage was applied
    UFUNCTION(BlueprintCallable, Category = Caombat)
    bool CauseDamageToActors(const TArray<AActor*>& TargetActors);

protected:

    void SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle);

};
