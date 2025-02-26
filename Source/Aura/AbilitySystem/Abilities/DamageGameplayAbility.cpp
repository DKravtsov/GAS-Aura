// Copyright 4sandwiches


#include "DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Game/AuraBlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UDamageGameplayAbility::CauseDamageToActor(AActor* TargetActor)
{
	// if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	// {
	// 	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	// 	SetupDamageTypes(EffectSpecHandle);
	// 	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	// }
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

	if (IsValid(TargetActor) && KnockBackChance > 0.f)
	{
		if (FMath::FRand() < KnockBackChance)
		{
			FRotator LookARotator = UKismetMathLibrary::FindLookAtRotation(GetAvatarActorFromActorInfo()->GetActorLocation(), TargetActor->GetActorLocation());
			if (bKnockBackPitchOverride)
			{
				LookARotator.Pitch = KnockBackPitch;
			}
			const FVector TargetDirection = LookARotator.Vector();
			Params.DeathImpulse = TargetDirection * DeathImpulseMagnitude;
			Params.KnockBackImpulse = TargetDirection * KnockBackImpulseMagnitude;
		}
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
