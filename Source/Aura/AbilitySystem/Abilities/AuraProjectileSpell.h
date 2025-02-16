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
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "CombatSocket"))
    FGameplayTag DefaultSocketTag;

public:

    UAuraProjectileSpell();

    UFUNCTION(BlueprintCallable)
    void SpawnProjectile(const FVector& TargetLocation, const FGameplayTag& SocketTag);

protected:

    //~ Begin of UGameplayAbility interface
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    //~ End of UGameplayAbility interface
    
};
