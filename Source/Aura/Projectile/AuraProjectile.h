// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AuraProjectile.generated.h"

UENUM()
enum class EProjectileEffectApplyingPolicy : uint8
{
    ApplyOnHit,
    ApplyOnOverlapping,
};

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<class USphereComponent> SphereComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UProjectileMovementComponent> ProjectileMovement; 

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TObjectPtr<class UNiagaraSystem> ImpactFX;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TObjectPtr<class USoundBase> ImpactSound;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    EProjectileEffectApplyingPolicy EffectApplyingPolicy = EProjectileEffectApplyingPolicy::ApplyOnHit;

public:

    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
    FDamageEffectParams DamageEffectParams;

private:

    bool bHit = false;

public:
    AAuraProjectile();

    USphereComponent* GetSphereComponent() const { return SphereComponent; }

    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

    //~ Begin of AActor interface

    virtual void Destroyed() override;
    bool OnHit(AActor* OtherActor, const FHitResult* HitResult = nullptr);

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

    virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
protected:
    virtual void BeginPlay() override;
    //~ End of AActor interface

    void PlayEffects();
};
