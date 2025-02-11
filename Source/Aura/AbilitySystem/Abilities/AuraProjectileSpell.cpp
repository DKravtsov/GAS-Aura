// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Characters/CombatInterface.h"
#include "Projectile/AuraProjectile.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


    // Native child classes should override ActivateAbility and call CommitAbility.
    // CommitAbility is used to do one last check for spending resources.
    // Previous versions of this function called CommitAbility but that prevents the callers
    // from knowing the result. Your override should call it and check the result.
    // Here is some starter code:

    //	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    //	{			
    //		constexpr bool bReplicateEndAbility = true;
    //		constexpr bool bWasCancelled = true;
    //		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    //	}

#if WITH_EDITOR
    if (ProjectileClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("ProjectileClass is not specified for %s"), *GetName());
        return;
    }
#endif //WITH_EDITOR

}

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
    const bool bIsServer = HasAuthority(&GetCurrentActivationInfoRef());

    if (bIsServer)
    {
        FVector SpawnLocation;
        if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
        {
            SpawnLocation = CombatInterface->GetCombatSocketLocation();
        }
        else
        {
            SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
        }
        FRotator Rotation = (TargetLocation - SpawnLocation).Rotation();
        Rotation.Pitch = 0.f;

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocation);
        SpawnTransform.SetRotation(Rotation.Quaternion());

        checkf(ProjectileClass, TEXT("ProjectileClass is not specified for %s"), *GetName());

        AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, 
            GetOwningActorFromActorInfo(), Cast<APawn>(GetAvatarActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

        const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
        Projectile->DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
        const float Damage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, AuraGameplayTags::SetByCaller_BaseDamage, Damage);

        Projectile->FinishSpawning(SpawnTransform);
    }
}
