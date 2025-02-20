// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

void UAuraGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ActivationPolicy == EAuraAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UAuraGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActivationPolicy == EAuraAbilityActivationPolicy::OnGiven && HasAuthority(&ActivationInfo))
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

void UAuraGameplayAbility::CauseDamageToActor(AActor* TargetActor)
{
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
		SetupDamageTypes(EffectSpecHandle);
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	}
}

bool UAuraGameplayAbility::CauseDamageToActors(const TArray<AActor*>& TargetActors)
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

void UAuraGameplayAbility::SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle)
{
	for (const auto& [DamageType, BaseDamage] : DamageTypes)
	{
		const float Damage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, DamageType, Damage);
	}
}
