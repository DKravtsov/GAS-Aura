// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
    FScalableFloat BaseDamage = 0.f;
};
