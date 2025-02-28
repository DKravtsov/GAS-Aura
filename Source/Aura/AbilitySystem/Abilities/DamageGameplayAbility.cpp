// Copyright 4sandwiches


#include "DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/CombatInterface.h"
#include "Game/AuraBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UDamageGameplayAbility::CauseDamageToActor(AActor* TargetActor)
{
	UAuraBlueprintFunctionLibrary::ApplyDamageEffect(MakeDamageEffectParams(TargetActor));
}

bool UDamageGameplayAbility::CauseDamageToActors(const TArray<AActor*>& TargetActors)
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

TArray<FGameplayEffectContextHandle> UDamageGameplayAbility::ApplyRadialDamageEffect(const FVector& Origin, const TArray<AActor*>& IgnoreActors)
{
	if (!HasAuthority(&CurrentActivationInfo))
		return {};
	
	// bind callbacks
	AActor* SourceAvatar = GetAvatarActorFromActorInfo();
    
	TArray<AActor*> Players;
	TArray<FGameplayEffectContextHandle> ReturnHandles;
	UAuraBlueprintFunctionLibrary::GetAllLivePlayersInRadius(SourceAvatar, Players, RadialDamageOuterRadius, Origin, IgnoreActors);
	for (AActor* Player : Players)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Player))
		{
			CombatInterface->GetActorTakeDamageDelegate().BindLambda(
				[&ReturnHandles, this, Origin](AActor* Actor, const float Damage)
				{
					auto Params = MakeDamageEffectParamsFromRadialDamage(Actor, Origin);
					UAuraBlueprintFunctionLibrary::ScaleDamageEffectParams(Params, Damage / Params.BaseDamage);
					ReturnHandles.Add(UAuraBlueprintFunctionLibrary::ApplyDamageEffect(Params));
				});
		}
	}
    
	// apply damage

	UGameplayStatics::ApplyRadialDamageWithFalloff(SourceAvatar, GetBaseDamage(GetAbilityLevel()), 0.f, Origin,
		RadialDamageInnerRadius, RadialDamageOuterRadius, RadialDamageFalloff, nullptr, IgnoreActors, SourceAvatar,
		CurrentActorInfo->PlayerController.Get());

	// unbind callbacks
	for (AActor* Player : Players)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Player))
		{
			CombatInterface->GetActorTakeDamageDelegate().Unbind();
		}
	}

	return ReturnHandles;
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
	FRotator LookARotator = UKismetMathLibrary::FindLookAtRotation(Origin, TargetActor->GetActorLocation());
	if (bKnockBackPitchOverride)
	{
		LookARotator.Pitch = KnockBackPitch;
	}
	const FVector TargetDirection = LookARotator.Vector();
	Params.DeathImpulse = TargetDirection * Params.DeathImpulseMagnitude;
	Params.KnockBackImpulse = TargetDirection * Params.KnockBackImpulseMagnitude;
}

FDamageEffectParams UDamageGameplayAbility::MakeDamageEffectParamsFromRadialDamage(AActor* TargetActor, const FVector& Origin) const
{
	FDamageEffectParams Params = MakeDamageEffectParams();	
	
	if (!IsValid(TargetActor))
		return Params;
	
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (Params.KnockBackChance > 0.f && FMath::FRand() <= Params.KnockBackChance)
	{
		AddKnockBackParams(TargetActor, Origin, Params);
	}
	return Params;
}

void UDamageGameplayAbility::SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle) const
{
	const float Damage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, DamageType, Damage);
}

void UDamageGameplayAbility::GetDynamicDescriptionInfo(FDynamicDescriptionInfo& OutDescriptionInfo, const int32 InLevel) const
{
	Super::GetDynamicDescriptionInfo(OutDescriptionInfo, InLevel);
	if (OutDescriptionInfo.bDamageText)
	{
		OutDescriptionInfo.Damage = GetBaseDamage(InLevel);
		OutDescriptionInfo.NextDamage = GetBaseDamage(InLevel + 1);
		OutDescriptionInfo.DamageType = GetDamageType();
	}
	if (OutDescriptionInfo.bDebuffText)
	{
		OutDescriptionInfo.DebuffDamage = DebuffDamage.GetValueAtLevel(InLevel);
		OutDescriptionInfo.DebuffChance = DebuffChance.GetValueAtLevel(InLevel);
		OutDescriptionInfo.DebuffFrequency = DebuffFrequency.GetValueAtLevel(InLevel);
		OutDescriptionInfo.DebuffDuration = DebuffDuration.GetValueAtLevel(InLevel);
		OutDescriptionInfo.NextDebuffDamage = DebuffDamage.GetValueAtLevel(InLevel + 1);
		OutDescriptionInfo.NextDebuffChance = DebuffChance.GetValueAtLevel(InLevel + 1);
		OutDescriptionInfo.NextDebuffFrequency = DebuffFrequency.GetValueAtLevel(InLevel + 1);
		OutDescriptionInfo.NextDebuffDuration = DebuffDuration.GetValueAtLevel(InLevel + 1);
	}
}
