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

	AnalyzeDescription();
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

float UAuraGameplayAbility::CalculateTotalDamage(const int32 InLevel) const
{
	float Damage = 0.f;
	for (const auto& [DamageType, BaseDamage] : DamageTypes)
	{
		Damage += BaseDamage.GetValueAtLevel(InLevel);
	}
	return Damage;
}

float UAuraGameplayAbility::GetDamageByType(const FGameplayTag& DamageType, const int32 InLevel) const
{
	return DamageTypes.FindRef(DamageType).GetValueAtLevel(InLevel);
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

void UAuraGameplayAbility::UpdateDescriptionTextArgs(FFormatNamedArguments& InArgs, const int32 InLevel) const
{
	InArgs.FindOrAdd(TEXT("Level")) = InLevel;
	InArgs.FindOrAdd(TEXT("NextLevel")) = InLevel + 1;

	static FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(1);

	InArgs.FindOrAdd(TEXT("Damage")) = FText::AsNumber(CalculateTotalDamage(InLevel), &Options);
	InArgs.FindOrAdd(TEXT("NextDamage")) = FText::AsNumber(CalculateTotalDamage(InLevel + 1), &Options);
	// InArgs.FindOrAdd(TEXT("Fire")) = FText::AsNumber(GetDamageByType(AuraGameplayTags::Damage_Fire, InLevel), &Options);
	// InArgs.FindOrAdd(TEXT("Lightning")) = FText::AsNumber(GetDamageByType(AuraGameplayTags::Damage_Lightning, InLevel), &Options);
	// InArgs.FindOrAdd(TEXT("Arcane")) = FText::AsNumber(GetDamageByType(AuraGameplayTags::Damage_Arcane, InLevel), &Options);

	InArgs.FindOrAdd(TEXT("Cost")) = FText::AsNumber(GetManaCost(InLevel), &Options);
	InArgs.FindOrAdd(TEXT("NextCost")) = FText::AsNumber(GetManaCost(InLevel + 1), &Options);

	InArgs.FindOrAdd(TEXT("Cooldown")) = FText::AsNumber(GetCooldown(InLevel), &Options);
	InArgs.FindOrAdd(TEXT("NextCooldown")) = FText::AsNumber(GetCooldown(InLevel + 1), &Options);
}

void UAuraGameplayAbility::UpdateDescriptionPredefinedTextArgs(FFormatNamedArguments& InArgs, const int32 InLevel) const
{
	InArgs.FindOrAdd(TEXT("Level")) = FText::Format(INVTEXT("<level>{0}</>"), InLevel);
	InArgs.FindOrAdd(TEXT("NextLevel")) = FText::Format(INVTEXT("<old>{0}</> > <level>{1}</>"), InLevel, InLevel + 1);

	static FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(1);

	if (bDamageText)
	{
		const float Damage = CalculateTotalDamage(InLevel);
		const float NextDamage = CalculateTotalDamage(InLevel + 1);
		const auto DamageText = FText::AsNumber(Damage, &Options);
		InArgs.FindOrAdd(TEXT("Damage")) = FText::Format(INVTEXT("<dmg>{0}</>"), DamageText);
		InArgs.FindOrAdd(TEXT("NextDamage")) = (!FMath::IsNearlyEqual(Damage, NextDamage, 0.05f))
			? FText::Format(INVTEXT("<old>{0}</> > <dmg>{1}</>"), DamageText, FText::AsNumber(NextDamage, &Options))
			: InArgs[TEXT("Damage")];
	}

	if (bDamageTypeFireText)
	{
		const float Damage = GetDamageByType(AuraGameplayTags::Damage_Fire, InLevel);
		const float NextDamage = GetDamageByType(AuraGameplayTags::Damage_Fire, InLevel + 1);
		const auto DamageText = FText::AsNumber(Damage, &Options);
		InArgs.FindOrAdd(TEXT("DamageFire")) = FText::Format(INVTEXT("<dmgfire>{0}</>"), DamageText);
		InArgs.FindOrAdd(TEXT("NextDamageFire")) = (!FMath::IsNearlyEqual(Damage, NextDamage, 0.05f))
			? FText::Format(INVTEXT("<old>{0}</> > <dmgfire>{1}</>"), DamageText, FText::AsNumber(NextDamage, &Options))
			: InArgs[TEXT("DamageFire")];
	}

	if (bDamageTypeArcaneText)
	{
		const float Damage = GetDamageByType(AuraGameplayTags::Damage_Arcane, InLevel);
		const float NextDamage = GetDamageByType(AuraGameplayTags::Damage_Arcane, InLevel + 1);
		const auto DamageText = FText::AsNumber(Damage, &Options);
		InArgs.FindOrAdd(TEXT("DamageArcane")) = FText::Format(INVTEXT("<dmgarc>{0}</>"), DamageText);
		InArgs.FindOrAdd(TEXT("NextDamageArcane")) = (!FMath::IsNearlyEqual(Damage, NextDamage, 0.05f))
			? FText::Format(INVTEXT("<old>{0}</> > <dmgarc>{1}</>"), DamageText, FText::AsNumber(NextDamage, &Options))
			: InArgs[TEXT("DamageArcane")];
	}

	if (bDamageTypeLightningText)
	{
		const float Damage = GetDamageByType(AuraGameplayTags::Damage_Lightning, InLevel);
		const float NextDamage = GetDamageByType(AuraGameplayTags::Damage_Lightning, InLevel + 1);
		const auto DamageText = FText::AsNumber(Damage, &Options);
		InArgs.FindOrAdd(TEXT("DamageLightning")) = FText::Format(INVTEXT("<dmglght>{0}</>"), DamageText);
		InArgs.FindOrAdd(TEXT("NextDamageLightning")) = (!FMath::IsNearlyEqual(Damage, NextDamage, 0.05f))
			? FText::Format(INVTEXT("<old>{0}</> > <dmglght>{1}</>"), DamageText, FText::AsNumber(NextDamage, &Options))
			: InArgs[TEXT("DamageLightning")];
	}

	if (bManaCostText)
	{
		const float ManaCost = GetManaCost(InLevel);
		const float NextManaCost = GetManaCost(InLevel + 1);
		const FText ManaCostText = FText::AsNumber(ManaCost, &Options);
		InArgs.FindOrAdd(TEXT("Cost")) = FText::Format(INVTEXT("<mana>{0}</>"), ManaCostText);
		InArgs.FindOrAdd(TEXT("NextCost")) = !FMath::IsNearlyEqual(ManaCost, NextManaCost, 0.05f)
			? FText::Format(INVTEXT("<old>{0}</> > <mana>{1}</>"), ManaCostText, FText::AsNumber(NextManaCost, &Options))
			: InArgs[TEXT("Cost")];
	}

	if (bCooldownText)
	{
		const float Cooldown = GetCooldown(InLevel);
		const float NextCooldown = GetCooldown(InLevel + 1);
		const FText CooldownText = FText::AsNumber(Cooldown, &Options);
	
		InArgs.FindOrAdd(TEXT("Cooldown")) = FText::Format(INVTEXT("<cooldown>{0}</>"), CooldownText);
		InArgs.FindOrAdd(TEXT("NextCooldown")) = !FMath::IsNearlyEqual(Cooldown, NextCooldown, 0.05f)
			? FText::Format(INVTEXT("<old>{0}</> > <cooldown>{1}</>"), CooldownText, FText::AsNumber(NextCooldown, &Options))
			: InArgs[TEXT("Cooldown")];
	}
}

void UAuraGameplayAbility::AnalyzeDescription()
{
	const FString Desc = Description.ToString();
	const FString NextDesc = NextLevelDescription.ToString();

	if (Desc.IsEmpty())
		return;

	UAuraGameplayAbility* CDO = GetClass()->GetDefaultObject<UAuraGameplayAbility>();
	if (CDO->bTextInitialized)
		return;
	
	CDO->bDamageText = Desc.Contains(TEXT("{Damage}"), ESearchCase::IgnoreCase);
	CDO->bManaCostText = Desc.Contains(TEXT("{Cost}"), ESearchCase::IgnoreCase);
	CDO->bCooldownText = Desc.Contains(TEXT("{Cooldown}"), ESearchCase::IgnoreCase);
	CDO->bDamageTypeFireText = Desc.Contains(TEXT("{DamageFire}"), ESearchCase::IgnoreCase);
	CDO->bDamageTypeLightningText = Desc.Contains(TEXT("{DamageLightning}"), ESearchCase::IgnoreCase);
	CDO->bDamageTypeArcaneText = Desc.Contains(TEXT("{DamageArcane}"), ESearchCase::IgnoreCase);
#if !WITH_EDITOR
  	CDO->bTextInitialized = true;
#endif
}

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

	UpdateDescriptionPredefinedTextArgs(Args, Level);
	OutDesc = FText::Format(SelectText(Description, DefaultTextFormat), Args);
	OutNextLevelDesc = FText::Format(SelectText(NextLevelDescription, DefaultNextTextFormat), Args);
}

#undef LOCTEXT_NAMESPACE