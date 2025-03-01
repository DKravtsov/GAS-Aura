// Copyright 4sandwiches


#include "AbilitySystem/Abilities/Spells/AuraFireBlast.h"

#include "Game/AuraBlueprintFunctionLibrary.h"
#include "Projectile/AuraFireBall.h"

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls() const
{
	if (!HasAuthority(&CurrentActivationInfo))
		return{};
	
	TArray<AAuraFireBall*> FireBalls;
	FireBalls.Reserve(NumFireBalls);

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	TArray<FVector> FireBallDirections = UAuraBlueprintFunctionLibrary::GetUniformSpreadOfDirections(Forward, 360.f, NumFireBalls + 1);

	APawn* FireBallOwnerPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	const FVector SpawnLocation = FireBallOwnerPawn->GetActorLocation();
	for (const auto&  SpawnDirection : FireBallDirections)
	{
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(FireBallClass, FTransform(),
			FireBallOwnerPawn, FireBallOwnerPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FireBall->DamageEffectParams = MakeDamageEffectParams();
		FireBall->ExplosionDamageParams = MakeDamageEffectParams();

		FireBall->FinishSpawning(FTransform(SpawnDirection.Rotation(), SpawnLocation));

		FireBalls.Add(FireBall);

		if (FireBalls.Num() >= NumFireBalls)
			break;
	}
	return FireBalls;
}
