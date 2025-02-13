// Copyright 4sandwiches


#include "Projectile/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Aura.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#define DEBUG_PROJECTILE 0

#if DEBUG_PROJECTILE
#include "Logs.h"
#endif

AAuraProjectile::AAuraProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
    SetRootComponent(SphereComponent);
    SphereComponent->SetCollisionProfileName(CollisionProfile_Projectile);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 550.f;
    ProjectileMovement->MaxSpeed = 550.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    
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

void AAuraProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (OtherActor == GetInstigator()) 
    {
        return;
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
            TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data);
        }

        SetLifeSpan(0.01);
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



