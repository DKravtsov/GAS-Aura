// Copyright 4sandwiches


#include "Projectile/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Aura.h"

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
    Super::BeginPlay();

}

void AAuraProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
}

void AAuraProjectile::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);
}



