// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SSkeletonWidget.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "AuraGameplayAbility"

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

FText UAuraGameplayAbility::GetDescription(const int32 Level) const
{
	const FTextFormat DefaultTextFormat = LOCTEXT("SpellDefaultDescFmt", "<Title>{title}</>\r\nLevel <Level>{Level}</>\r\nDescription:\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla condimentum augue id purus porta lacinia. Praesent non lorem posuere, interdum sapien sit amet, efficitur tortor. Duis scelerisque tortor velit. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Cras auctor ex vel nisl vehicula, convallis euismod.");

	static const FText DefaultSpellName = LOCTEXT("SpellDefaultName", "Default Spell Name");
	return FText::Format(DefaultTextFormat, DefaultSpellName, Level);
}

FText UAuraGameplayAbility::GetNextLevelDescription(const int32 Level) const
{
	const FTextFormat DefaultTextFormat = LOCTEXT("SpellDefaultDescNextFmt", "Next level: <Level>{Level}</>\r\nCauses much more damage");
	return FText::Format(DefaultTextFormat, Level);
}

void UAuraGameplayAbility::SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle)
{
	for (const auto& [DamageType, BaseDamage] : DamageTypes)
	{
		const float Damage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, DamageType, Damage);
	}
}

#undef LOCTEXT_NAMESPACE