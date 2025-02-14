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

    // The gameplay tag to extract correct socket from the character where the projectile will be spawned
    // Note: It must bu the same tag that is used in AttackMontage array in EnemyCharacter
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag MontageTag;

public:

    UFUNCTION(BlueprintCallable)
    void SpawnProjectile(const FVector& TargetLocation);

protected:

    //~ Begin of UGameplayAbility interface
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    //~ End of UGameplayAbility interface
    
};
