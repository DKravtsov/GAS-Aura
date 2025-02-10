// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraGameplayAbility
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class AAuraProjectile> ProjectileClass;

public:

    UFUNCTION(BlueprintCallable)
    void SpawnProjectile();

protected:

    //~ Begin of UGameplayAbility interface
    void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    //~ End of UGameplayAbility interface
};
