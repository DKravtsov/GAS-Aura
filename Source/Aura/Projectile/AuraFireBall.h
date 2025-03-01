// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Projectile/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, Category="FireBall")
	FDamageEffectParams ExplosionDamageParams;
	
public:

	AAuraFireBall(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

    virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void BeginPlay() override;
};
