// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UDamageGameplayAbility
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<class AAuraProjectile> ProjectileClass;

    // The gameplay tag to extract correct socket from the character where the projectile will be spawned
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (Categories = "CombatSocket"))
    FGameplayTag DefaultSocketTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta=(InlineEditConditionToggle))
    bool bOverrideLaunchPitchAngle = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta =(EditCondition="bOverrideLaunchPitchAngle"))
    float LaunchPitchAngle = 0.f;

public:

    UAuraProjectileSpell();

    UFUNCTION(BlueprintCallable)
    void SpawnProjectile(const FVector& TargetLocation, const FGameplayTag& SocketTag);

protected:

    //~ Begin of UGameplayAbility interface
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    //~ End of UGameplayAbility interface
    
};
