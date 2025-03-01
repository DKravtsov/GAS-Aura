// Copyright 4sandwiches


#include "UAuraFireBolt.h"

#include "Characters/CombatInterface.h"
#include "Game/AuraBlueprintFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Projectile/AuraProjectile.h"

void UUAuraFireBolt::SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, AActor* HomingTargetActor)
{
	if (HasAuthority(&GetCurrentActivationInfoRef()))
	{
		const FVector SpawnLocation =
			ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag.IsValid() ? SocketTag : DefaultSocketTag);
		FRotator Rotation = (TargetLocation - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		if (bOverrideLaunchPitchAngle)
		{
			Rotation.Pitch = LaunchPitchAngle;
		}

		checkf(ProjectileClass, TEXT("ProjectileClass is not specified for %s"), *GetName());
		const bool bHasValidTarget = IsValid(HomingTargetActor) && HomingTargetActor->Implements<UCombatInterface>();
		
		const FVector Forward = Rotation.Vector();
		UWorld* World = GetAvatarActorFromActorInfo()->GetWorld();
		//::DrawDebugDirectionalArrow(World, SpawnLocation, SpawnLocation + Forward * 100.f, 5.f, FColor::White, false, 60.f, SDPG_World, 2.f);

		const int32 NumProjectiles = GetNumProjectiles(GetAbilityLevel());

		TArray<FVector> Directions = UAuraBlueprintFunctionLibrary::GetUniformSpreadOfDirections(Forward, ProjectileSpread, NumProjectiles);
		for (const FVector& Direction : Directions)
		{
			//::DrawDebugDirectionalArrow(World, SpawnLocation, SpawnLocation + Direction * 75.f, 5.f, FColor::Silver, false, 60.f, SDPG_World, 1.f);

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SpawnLocation);
			SpawnTransform.SetRotation(Direction.ToOrientationQuat());

			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, 
				GetOwningActorFromActorInfo(), Cast<APawn>(GetAvatarActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			Projectile->DamageEffectParams = MakeDamageEffectParams(nullptr);

			if (bUseHomingProjectiles)
			{
				const float Magnitude = UAuraBlueprintFunctionLibrary::FRandInRange(HomingAccelerationMinMax);
				if (bHasValidTarget)
				{
					Projectile->SetHominTargetActor(HomingTargetActor, Magnitude);
				}
				else
				{
					Projectile->SetHomingTargetLocation(TargetLocation, Magnitude);
				}
			}
			
			Projectile->FinishSpawning(SpawnTransform);
		}

	}
}

int32 UUAuraFireBolt::GetNumProjectiles(const int32 InLevel) const
{
	return FMath::Min(MaxNumProjectiles, InLevel/2+1);
}
