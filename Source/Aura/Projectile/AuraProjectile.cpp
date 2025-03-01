// Copyright 4sandwiches


#include "Projectile/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Aura.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

#define DEBUG_PROJECTILE 0

#if DEBUG_PROJECTILE
#include "Logs.h"
#endif

AAuraProjectile::AAuraProjectile(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
    SetRootComponent(SphereComponent);

    // #todo: make different collision profiles for overlapping and blocking policies
    SphereComponent->SetCollisionProfileName(CollisionProfile_Projectile);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 550.f;
    ProjectileMovement->MaxSpeed = 550.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    
    SetReplicatingMovement(true);
    
    InitialLifeSpan = 5.f;
}

void AAuraProjectile::BeginPlay()
{
#if DEBUG_PROJECTILE
      LOG_NETFUNCTIONCALL_MSG(TEXT("time=%s"), *FString::SanitizeFloat(GetWorld()->GetTimeSeconds()));
#endif


    Super::BeginPlay();

    SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AAuraProjectile::Destroyed()
{
#if DEBUG_PROJECTILE
      LOG_NETFUNCTIONCALL_MSG(TEXT("bHit=%s, time=%s"), (bHit ? TEXT("true") : TEXT("false")), *FString::SanitizeFloat(GetWorld()->GetTimeSeconds()));
#endif


    if (!bHit)
    {
        PlayEffects();
    }
    Super::Destroyed();
}

bool AAuraProjectile::OnHit(AActor* OtherActor, const FHitResult* HitResult)
{
    if (OtherActor == GetInstigator() || UAuraBlueprintFunctionLibrary::AreFriendly(OtherActor, GetInstigator())) 
    {
        return false;
    }

#if DEBUG_PROJECTILE
      LOG_NETFUNCTIONCALL;
#endif


    bHit = true;
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PlayEffects();
    if (HasAuthority())
    {
        if (auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
        {
            DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
            DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
            if (FMath::FRand() <= DamageEffectParams.KnockBackChance)
            {
                FRotator Rotation = GetActorRotation();
                Rotation.Pitch = 45.f;
                DamageEffectParams.KnockBackImpulse = Rotation.Vector() * DamageEffectParams.KnockBackImpulseMagnitude;
            }
            if (HitResult)
            {
                DamageEffectParams.SetHitResult(*HitResult);
            }
            UAuraBlueprintFunctionLibrary::ApplyDamageEffect(DamageEffectParams);
        }

        SetLifeSpan(0.01);
    }
    return true;
}

void AAuraProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (!bHit && EffectApplyingPolicy == EProjectileEffectApplyingPolicy::ApplyOnOverlapping)
    {
        OnHit(OtherActor);
    }

}

void AAuraProjectile::PlayEffects()
{
#if DEBUG_PROJECTILE
      LOG_NETFUNCTIONCALL;
#endif


    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, GetActorLocation());
}

void AAuraProjectile::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);
}

void AAuraProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
    bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    if (EffectApplyingPolicy == EProjectileEffectApplyingPolicy::ApplyOnHit && !bHit)
    {
        bHit = true;
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        OnHit(Other, &Hit);

        if (HasAuthority())
        {
            SetLifeSpan(0.01);
        }
    }
}

void AAuraProjectile::SetHomingTargetActor(const AActor* TargetActor, const float HomingAccelerationMagnitude)
{
    check(IsValid(TargetActor));
    GetProjectileMovement()->HomingTargetComponent = TargetActor->GetRootComponent();
    GetProjectileMovement()->bIsHomingProjectile = true;
    GetProjectileMovement()->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
}

void AAuraProjectile::SetHomingTargetLocation(const FVector& TargetLocation, const float HomingAccelerationMagnitude)
{
    HomingTargetSceneComponent = NewObject<USceneComponent>(this, USceneComponent::StaticClass());
    HomingTargetSceneComponent->SetWorldLocation(TargetLocation);
    GetProjectileMovement()->HomingTargetComponent = HomingTargetSceneComponent;
    GetProjectileMovement()->bIsHomingProjectile = true;
    GetProjectileMovement()->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
}



