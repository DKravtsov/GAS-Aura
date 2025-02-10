// Copyright 4sandwiches


#include "Projectile/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Aura.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Logs.h"

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
    
    SetLifeSpan(5.f);
}

void AAuraProjectile::BeginPlay()
{
    LOG_NETFUNCTIONCALL_MSG(TEXT("time=%s"), *FString::SanitizeFloat(GetWorld()->GetTimeSeconds()));

    Super::BeginPlay();

    SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AAuraProjectile::Destroyed()
{
    LOG_NETFUNCTIONCALL_MSG(TEXT("bHit=%s, time=%s"), (bHit ? TEXT("true") : TEXT("false")), *FString::SanitizeFloat(GetWorld()->GetTimeSeconds()));

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

    LOG_NETFUNCTIONCALL;

    bHit = true;
    PlayEffects();
    if (HasAuthority())
    {
        Destroy();
    }

}

void AAuraProjectile::PlayEffects()
{
    LOG_NETFUNCTIONCALL;

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, GetActorLocation());
}

//void AAuraProjectile::NotifyActorEndOverlap(AActor* OtherActor)
//{
//    Super::NotifyActorEndOverlap(OtherActor);
//}



