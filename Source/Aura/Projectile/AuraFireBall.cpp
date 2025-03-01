// Copyright 4sandwiches


#include "Projectile/AuraFireBall.h"

#include "Aura.h"
#include "Components/SphereComponent.h"

AAuraFireBall::AAuraFireBall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetSphereComponent()->SetCollisionProfileName(CollisionProfile_FireBall);
}

void AAuraFireBall::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AActor::NotifyActorBeginOverlap(OtherActor);
}

void AAuraFireBall::NotifyActorEndOverlap(AActor* OtherActor)
{
	AActor::NotifyActorEndOverlap(OtherActor);
}

void AAuraFireBall::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
}
