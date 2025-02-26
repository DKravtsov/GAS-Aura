// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

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

	FDynamicDescriptionInfo::AnalyzeDescription(GetClass());
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

#pragma region Description implementation

void UAuraGameplayAbility::GetDescription(const int32 Level, FText& OutDesc, FText& OutNextLevelDesc) const
{
	static auto SelectText = [](const FText& InText, const auto& Default)
	{
		return !InText.IsEmpty() ? InText : Default;
	};
	static const FTextFormat DefaultTextFormat = LOCTEXT("SpellDefaultDescFmt", "<Title>{Name}</>\r\nLevel <Level>{Level}</>\r\nDescription:\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla condimentum augue id purus porta lacinia. Praesent non lorem posuere, interdum sapien sit amet, efficitur tortor. Duis scelerisque tortor velit. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Cras auctor ex vel nisl vehicula, convallis euismod.");
	static const FTextFormat DefaultNextTextFormat = LOCTEXT("SpellDefaultDescNextFmt", "Next level: <Level>{Level}</>\r\nCauses much more <damage>damage</>.");
	static const FText DefaultSpellName = LOCTEXT("SpellDefaultName", "Default Spell Name");

	// Replace all "variables" in the description with their values


	FFormatNamedArguments Args;
	Args.Add(TEXT("Name"), SelectText(AbilityName.ToUpper(), DefaultSpellName));

	DynamicDescriptionInfo.UpdateDescriptionPredefinedTextArgs(Args, this, Level);
	OutDesc = FText::Format(SelectText(Description, DefaultTextFormat), Args);
	OutNextLevelDesc = FText::Format(SelectText(NextLevelDescription, DefaultNextTextFormat), Args);
}

void UAuraGameplayAbility::GetDynamicDescriptionInfo(FDynamicDescriptionInfo& OutDescriptionInfo, const int32 InLevel) const
{
	UAuraGameplayAbility* CDO = GetClass()->GetDefaultObject<UAuraGameplayAbility>();
	if (CDO == nullptr)
		return;
	OutDescriptionInfo = CDO->DynamicDescriptionInfo;
	
	if (DynamicDescriptionInfo.bManaCostText)
	{
		OutDescriptionInfo.ManaCost = GetManaCost(InLevel);
		OutDescriptionInfo.NextManaCost = GetManaCost(InLevel + 1);
	}

	if (DynamicDescriptionInfo.bCooldownText)
	{
		OutDescriptionInfo.Cooldown = GetCooldown(InLevel);
		OutDescriptionInfo.NextCooldown = GetCooldown(InLevel + 1);
	}
}


#pragma endregion 

#undef LOCTEXT_NAMESPACE