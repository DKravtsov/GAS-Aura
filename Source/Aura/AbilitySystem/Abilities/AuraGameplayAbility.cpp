// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraGameplayAbility::CauseDamageToActor(AActor* TargetActor)
{
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
		SetupDamageTypes(EffectSpecHandle);
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	}
}

void UAuraGameplayAbility::CauseDamageToActors(const TArray<AActor*>& TargetActors)
{
	for (AActor* TargetActor : TargetActors)
	{
		CauseDamageToActor(TargetActor);
	}
}

void UAuraGameplayAbility::SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle)
{
	for (const auto& [DamageType, BaseDamage] : DamageTypes)
	{
		const float Damage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, DamageType, Damage);
	}
}
