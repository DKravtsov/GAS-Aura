// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "SSkeletonWidget.h"
#include "AbilitySystem/AuraAttributeSet.h"
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

void UAuraGameplayAbility::FDynamicDescriptionInfo::UpdateDescriptionPredefinedTextArgs(FFormatNamedArguments& InArgs, const UAuraGameplayAbility* Ability, const int32 InLevel)
{
	if (Ability == nullptr)
		return;
	
	FDynamicDescriptionInfo DescriptionInfo;
	Ability->GetDynamicDescriptionInfo(DescriptionInfo, InLevel);

	InArgs.FindOrAdd(TEXT("Level")) = FText::Format(INVTEXT("<level>{0}</>"), InLevel);
	InArgs.FindOrAdd(TEXT("NextLevel")) = FText::Format(INVTEXT("<old>{0}</> > <level>{1}</>"), InLevel, InLevel + 1);

	static const FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(1);

	if (DescriptionInfo.bDamageText)
	{
		static const FTextFormat GenericDamageTypeFmt = LOCTEXT("GenericDamageFmt", "<dmg>{0}</> damage");
		static const FTextFormat FireDamageTypeFmt = LOCTEXT("FireDamageFmt", "<dmgfire>{0} fire</> damage");
		static const FTextFormat LightningDamageTypeFmt = LOCTEXT("LightningDamageFmt", "<dmglght>{0} lightning</> damage");
		static const FTextFormat ArcaneDamageTypeFmt = LOCTEXT("ArcaneDamageFmt", "<dmgarc>{0} arcane</> daamage");

		static const FTextFormat NextGenericDamageTypeFmt = LOCTEXT("NextGenericDamageFmt", "<old>{1}</> > <dmg>{0}</> damage");
		static const FTextFormat NextFireDamageTypeFmt = LOCTEXT("NextFireDamageFmt", "<old>{1}</> > <dmgfire>{0} fire</> damage");
		static const FTextFormat NextLightningDamageTypeFmt = LOCTEXT("NextLightningDamageFmt", "<old>{1}</> > <dmglght>{0} lightning</> damage");
		static const FTextFormat NextArcaneDamageTypeFmt = LOCTEXT("NextArcaneDamageFmt", "<old>{1}</> > <dmgarc>{0} arcane</> damage");

		const FTextFormat* DamageFmt;
		const FTextFormat* NextDamageFmt;
		const bool bEqualDamage = FMath::IsNearlyEqual(DescriptionInfo.Damage, DescriptionInfo.NextDamage, 0.05f);
		if (DescriptionInfo.DamageType == AuraGameplayTags::Damage_Fire)
		{
			DamageFmt = &FireDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &FireDamageTypeFmt : &NextFireDamageTypeFmt;
		}
		else if (DescriptionInfo.DamageType == AuraGameplayTags::Damage_Lightning)
		{
			DamageFmt = &LightningDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &LightningDamageTypeFmt: &NextLightningDamageTypeFmt;
			
		}
		else if (DescriptionInfo.DamageType == AuraGameplayTags::Damage_Arcane)
		{
			DamageFmt = &ArcaneDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &ArcaneDamageTypeFmt : &NextArcaneDamageTypeFmt;
		}
		else
		{
			DamageFmt = &GenericDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &GenericDamageTypeFmt : &NextGenericDamageTypeFmt;
		}
		
		const auto DamageText = FText::AsNumber(DescriptionInfo.Damage, &Options);
		const auto NextDamageText = FText::AsNumber(DescriptionInfo.NextDamage, &Options);
		InArgs.FindOrAdd(TEXT("Damage")) = FText::Format(*DamageFmt, DamageText);
		InArgs.FindOrAdd(TEXT("NextDamage")) = FText::Format(*NextDamageFmt, NextDamageText, DamageText);
	}

	if (DescriptionInfo.bManaCostText)
	{
		const FText ManaCostText = FText::AsNumber(DescriptionInfo.ManaCost, &Options);
		InArgs.FindOrAdd(TEXT("Cost")) = FText::Format(INVTEXT("<mana>{0}</>"), ManaCostText);
		InArgs.FindOrAdd(TEXT("NextCost")) = !FMath::IsNearlyEqual(DescriptionInfo.ManaCost, DescriptionInfo.NextManaCost, 0.05f)
			? FText::Format(INVTEXT("<old>{0}</> > <mana>{1}</>"), ManaCostText, FText::AsNumber(DescriptionInfo.NextManaCost, &Options))
			: InArgs[TEXT("Cost")];
	}

	if (DescriptionInfo.bCooldownText)
	{
		const FText CooldownText = FText::AsNumber(DescriptionInfo.Cooldown, &Options);
		InArgs.FindOrAdd(TEXT("Cooldown")) = FText::Format(INVTEXT("<cooldown>{0}</>"), CooldownText);
		InArgs.FindOrAdd(TEXT("NextCooldown")) = !FMath::IsNearlyEqual(DescriptionInfo.Cooldown, DescriptionInfo.NextCooldown, 0.05f)
			? FText::Format(INVTEXT("<old>{0}</> > <cooldown>{1}</>"), CooldownText, FText::AsNumber(DescriptionInfo.NextCooldown, &Options))
			: InArgs[TEXT("Cooldown")];
	}
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

void UAuraGameplayAbility::FDynamicDescriptionInfo::AnalyzeDescription(TSubclassOf<UAuraGameplayAbility> AbilityClass)
{
	UAuraGameplayAbility* CDO = AbilityClass->GetDefaultObject<UAuraGameplayAbility>();
	if (CDO == nullptr || CDO->DynamicDescriptionInfo.bTextInitialized || CDO->Description.IsEmpty())
		return;

	const FString Desc = CDO->Description.ToString();
	const FString NextDesc = CDO->NextLevelDescription.ToString();

	CDO->DynamicDescriptionInfo.bDamageText = Desc.Contains(TEXT("{Damage}"), ESearchCase::IgnoreCase);
	CDO->DynamicDescriptionInfo.bManaCostText = Desc.Contains(TEXT("{Cost}"), ESearchCase::IgnoreCase);
	CDO->DynamicDescriptionInfo.bCooldownText = Desc.Contains(TEXT("{Cooldown}"), ESearchCase::IgnoreCase);
#if !WITH_EDITOR
  	CDO->DescriptionInfo.bTextInitialized = true;
#endif
}

#pragma endregion 

#undef LOCTEXT_NAMESPACE