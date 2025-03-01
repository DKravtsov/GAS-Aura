// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

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

float UAuraGameplayAbility::GetManaCost(const int32 InLevel) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (const auto Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				if (Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost))
				{
					break;
				}
			}
		}
	}
	return -ManaCost;
}

float UAuraGameplayAbility::GetCooldown(const int32 InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}

UAuraAbilitySystemComponent* UAuraGameplayAbility::GetAuraAbilitySystemComponent() const
{
	return Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

void UAuraGameplayAbility::GetDescription_Implementation(const int32 Level, FText& OutDesc, FText& OutNextLevelDesc) const
{
	static const FTextFormat DefaultTextFormat = INVTEXT("<Title>DEFAULT SPELL</>\r\nLevel <Level>{0}</>\r\nDescription:\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla condimentum augue id purus porta lacinia. Praesent non lorem posuere, interdum sapien sit amet, efficitur tortor. Duis scelerisque tortor velit. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Cras auctor ex vel nisl vehicula, convallis euismod.");
	static const FTextFormat DefaultNextTextFormat = INVTEXT("<Title>Next Level</>Level: <Level>{Level}</>\r\nCauses much more <damage>damage</>.");

	OutDesc = FText::Format(DefaultTextFormat, Level);
	OutNextLevelDesc = FText::Format(DefaultNextTextFormat, Level + 1);
}
