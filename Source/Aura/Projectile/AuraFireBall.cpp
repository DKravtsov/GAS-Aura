// Copyright 4sandwiches


#include "Projectile/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Aura.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "Components/SphereComponent.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

AAuraFireBall::AAuraFireBall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetSphereComponent()->SetCollisionProfileName(CollisionProfile_FireBall);
}

void AAuraFireBall::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AActor::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor == GetInstigator() || UAuraBlueprintFunctionLibrary::AreFriendly(OtherActor, GetInstigator())) 
	{
		return ;
	}

	if (HasAuthority())
	{
		if (auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			UAuraBlueprintFunctionLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
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

void AAuraFireBall::PlayEffects()
{
	if (IsValid(GetOwner()))
	{
		FGameplayCueParameters Params;
		Params.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), AuraGameplayTags::GameplayCue_FireBlast, Params);
	}
}
