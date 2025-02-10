// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<class USphereComponent> SphereComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UProjectileMovementComponent> ProjectileMovement; 

    UPROPERTY(EditAnywhere)
    TObjectPtr<class UNiagaraSystem> ImpactFX;

    UPROPERTY(EditAnywhere)
    TObjectPtr<class USoundBase> ImpactSound;

    bool bHit = false;

public:
    AAuraProjectile();

    USphereComponent* GetSphereComponent() const { return SphereComponent; }

    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

    //~ Begin of AActor interface

    void Destroyed() override;

    void NotifyActorBeginOverlap(AActor* OtherActor) override;
    //void NotifyActorEndOverlap(AActor* OtherActor) override;
protected:
    void BeginPlay() override;
    //~ End of AActor interface

    void PlayEffects();
};
