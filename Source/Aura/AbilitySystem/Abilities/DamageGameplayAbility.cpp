// Copyright 4sandwiches


#include "DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/CombatInterface.h"
#include "Game/AuraBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UDamageGameplayAbility::CauseDamageToActor(AActor* TargetActor) const
{
	UAuraBlueprintFunctionLibrary::ApplyDamageEffect(MakeDamageEffectParams(TargetActor));
}

bool UDamageGameplayAbility::CauseDamageToActors(const TArray<AActor*>& TargetActors) const
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	bool bResult = false;
	for (AActor* TargetActor : TargetActors)
	{
		if (!UAuraBlueprintFunctionLibrary::AreFriendly(Avatar, TargetActor))
		{
			CauseDamageToActor(TargetActor);
			bResult = true;
		}
	}
	return bResult;
}

void UDamageGameplayAbility::CauseDamageToActorsInRadius(const FVector& Origin, const TArray<AActor*>& IgnoreActors) const
{
	if (!HasAuthority(&CurrentActivationInfo))
		return;
	
	UAuraBlueprintFunctionLibrary::ApplyRadialDamageEffect(MakeDamageEffectParams(), Origin,
		RadialDamageInnerRadius, RadialDamageOuterRadius, RadialDamageFalloff, IgnoreActors, bKnockBackPitchOverride, KnockBackPitch);
}

float UDamageGameplayAbility::GetBaseDamage(const int32 InLevel) const
{
	return BaseDamage.GetValueAtLevel(InLevel);
}

FDamageEffectParams UDamageGameplayAbility::MakeDamageEffectParams(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.DamageEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = GetBaseDamage(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance.GetValueAtLevel(Params.AbilityLevel);
	Params.DebuffDamage = DebuffDamage.GetValueAtLevel(Params.AbilityLevel);
	Params.DebuffDuration = DebuffDuration.GetValueAtLevel(Params.AbilityLevel);
	Params.DebuffFrequency = DebuffFrequency.GetValueAtLevel(Params.AbilityLevel);
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockBackImpulseMagnitude = KnockBackImpulseMagnitude;
	Params.KnockBackChance = KnockBackChance;

	if (IsValid(TargetActor) && KnockBackChance > 0.f && FMath::FRand() <= KnockBackChance)
	{
		AddKnockBackParams(TargetActor, GetAvatarActorFromActorInfo()->GetActorLocation(), Params);
	}

	return Params;
}

void UDamageGameplayAbility::AddKnockBackParams(const AActor* TargetActor, const FVector& Origin, FDamageEffectParams& Params) const
{
	UAuraBlueprintFunctionLibrary::AddKnockBackParams(TargetActor, Origin, Params, bKnockBackPitchOverride, KnockBackPitch);
}

void UDamageGameplayAbility::SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle) const
{
	const float Damage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, DamageType, Damage);
}
