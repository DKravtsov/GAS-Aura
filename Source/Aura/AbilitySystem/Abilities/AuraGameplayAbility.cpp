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

	FDescriptionInfo::AnalyzeDescription(GetClass());
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

float UAuraGameplayAbility::GetBaseDamage(const int32 InLevel) const
{
	return BaseDamage.GetValueAtLevel(InLevel);
}

void UAuraGameplayAbility::SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle) const
{
	const float Damage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, DamageType, Damage);
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

	DescriptionInfo.UpdateDescriptionPredefinedTextArgs(Args, this, Level);
	OutDesc = FText::Format(SelectText(Description, DefaultTextFormat), Args);
	OutNextLevelDesc = FText::Format(SelectText(NextLevelDescription, DefaultNextTextFormat), Args);
}

void UAuraGameplayAbility::FDescriptionInfo::UpdateDescriptionTextArgs(FFormatNamedArguments& InArgs, const UAuraGameplayAbility* Ability, const int32 InLevel)
{
	InArgs.FindOrAdd(TEXT("Level")) = InLevel;
	InArgs.FindOrAdd(TEXT("NextLevel")) = InLevel + 1;

	static FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(1);

	InArgs.FindOrAdd(TEXT("Damage")) = FText::AsNumber(Ability->GetBaseDamage(InLevel), &Options);
	InArgs.FindOrAdd(TEXT("NextDamage")) = FText::AsNumber(Ability->GetBaseDamage(InLevel + 1), &Options);

	InArgs.FindOrAdd(TEXT("Cost")) = FText::AsNumber(Ability->GetManaCost(InLevel), &Options);
	InArgs.FindOrAdd(TEXT("NextCost")) = FText::AsNumber(Ability->GetManaCost(InLevel + 1), &Options);

	InArgs.FindOrAdd(TEXT("Cooldown")) = FText::AsNumber(Ability->GetCooldown(InLevel), &Options);
	InArgs.FindOrAdd(TEXT("NextCooldown")) = FText::AsNumber(Ability->GetCooldown(InLevel + 1), &Options);
}

void UAuraGameplayAbility::FDescriptionInfo::UpdateDescriptionPredefinedTextArgs(FFormatNamedArguments& InArgs, const UAuraGameplayAbility* Ability, const int32 InLevel)
{
	if (Ability == nullptr)
		return;
	
	UAuraGameplayAbility* CDO = Ability->GetClass()->GetDefaultObject<UAuraGameplayAbility>();
	check(CDO != nullptr);

	InArgs.FindOrAdd(TEXT("Level")) = FText::Format(INVTEXT("<level>{0}</>"), InLevel);
	InArgs.FindOrAdd(TEXT("NextLevel")) = FText::Format(INVTEXT("<old>{0}</> > <level>{1}</>"), InLevel, InLevel + 1);

	static const FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(1);

	if (CDO->DescriptionInfo.bDamageText)
	{
		const float Damage = Ability->GetBaseDamage(InLevel);
		const float NextDamage = Ability->GetBaseDamage(InLevel + 1);
		const FGameplayTag& DamageType = Ability->GetDamageType();
	
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
		const bool bEqualDamage = FMath::IsNearlyEqual(Damage, NextDamage, 0.05f);
		if (DamageType == AuraGameplayTags::Damage_Fire)
		{
			DamageFmt = &FireDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &FireDamageTypeFmt : &NextFireDamageTypeFmt;
		}
		else if (DamageType == AuraGameplayTags::Damage_Lightning)
		{
			DamageFmt = &LightningDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &LightningDamageTypeFmt: &NextLightningDamageTypeFmt;
			
		}
		else if (DamageType == AuraGameplayTags::Damage_Arcane)
		{
			DamageFmt = &ArcaneDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &ArcaneDamageTypeFmt : &NextArcaneDamageTypeFmt;
		}
		else
		{
			DamageFmt = &GenericDamageTypeFmt;
			NextDamageFmt = bEqualDamage ? &GenericDamageTypeFmt : &NextGenericDamageTypeFmt;
		}
		
		const auto DamageText = FText::AsNumber(Damage, &Options);
		const auto NextDamageText = FText::AsNumber(NextDamage, &Options);
		InArgs.FindOrAdd(TEXT("Damage")) = FText::Format(*DamageFmt, DamageText);
		InArgs.FindOrAdd(TEXT("NextDamage")) = FText::Format(*NextDamageFmt, NextDamageText, DamageText);
	}

	if (CDO->DescriptionInfo.bManaCostText)
	{
		const float ManaCost = Ability->GetManaCost(InLevel);
		const float NextManaCost = Ability->GetManaCost(InLevel + 1);
		const FText ManaCostText = FText::AsNumber(ManaCost, &Options);
		InArgs.FindOrAdd(TEXT("Cost")) = FText::Format(INVTEXT("<mana>{0}</>"), ManaCostText);
		InArgs.FindOrAdd(TEXT("NextCost")) = !FMath::IsNearlyEqual(ManaCost, NextManaCost, 0.05f)
			? FText::Format(INVTEXT("<old>{0}</> > <mana>{1}</>"), ManaCostText, FText::AsNumber(NextManaCost, &Options))
			: InArgs[TEXT("Cost")];
	}

	if (CDO->DescriptionInfo.bCooldownText)
	{
		const float Cooldown = Ability->GetCooldown(InLevel);
		const float NextCooldown = Ability->GetCooldown(InLevel + 1);
		const FText CooldownText = FText::AsNumber(Cooldown, &Options);
	
		InArgs.FindOrAdd(TEXT("Cooldown")) = FText::Format(INVTEXT("<cooldown>{0}</>"), CooldownText);
		InArgs.FindOrAdd(TEXT("NextCooldown")) = !FMath::IsNearlyEqual(Cooldown, NextCooldown, 0.05f)
			? FText::Format(INVTEXT("<old>{0}</> > <cooldown>{1}</>"), CooldownText, FText::AsNumber(NextCooldown, &Options))
			: InArgs[TEXT("Cooldown")];
	}
}

void UAuraGameplayAbility::FDescriptionInfo::AnalyzeDescription(TSubclassOf<UAuraGameplayAbility> AbilityClass)
{
	UAuraGameplayAbility* CDO = AbilityClass->GetDefaultObject<UAuraGameplayAbility>();
	if (CDO == nullptr || CDO->DescriptionInfo.bTextInitialized || CDO->Description.IsEmpty())
		return;

	const FString Desc = CDO->Description.ToString();
	const FString NextDesc = CDO->NextLevelDescription.ToString();

	CDO->DescriptionInfo.bDamageText = Desc.Contains(TEXT("{Damage}"), ESearchCase::IgnoreCase);
	CDO->DescriptionInfo.bManaCostText = Desc.Contains(TEXT("{Cost}"), ESearchCase::IgnoreCase);
	CDO->DescriptionInfo.bCooldownText = Desc.Contains(TEXT("{Cooldown}"), ESearchCase::IgnoreCase);
#if !WITH_EDITOR
  	CDO->DescriptionInfo.bTextInitialized = true;
#endif
}

#undef LOCTEXT_NAMESPACE