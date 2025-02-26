// Copyright 4sandwiches

#include "SpellDescription.h"
#include "AuraGameplayTags.h"
#include "AuraGameplayAbility.h"

#define LOCTEXT_NAMESPACE "AuraGameplayAbility"

// VarName - DescriptionInfo.VarName
// AliasName - how it will be named in formats
#define IMPLEMENT_ARGS(VarName, AliasName, TextFmt, NextTextFmt) \
	{ \
		static const FTextFormat Fmt = TextFmt; \
		static const FTextFormat NextFmt = NextTextFmt; \
		const FText ValueText = FText::AsNumber(DescriptionInfo.VarName, &Options); \
		const FText NextValueText = FText::AsNumber(DescriptionInfo.Next##VarName, &Options); \
		InArgs.Add(TEXT(AliasName),  FText::FormatOrdered(Fmt, ValueText)); \
		const bool bEqual = FMath::IsNearlyEqual(DescriptionInfo.VarName, DescriptionInfo.Next##VarName, 0.05f); \
		InArgs.Add(TEXT("Next" AliasName), FText::FormatOrdered(bEqual ? Fmt : NextFmt, NextValueText, ValueText)); \
	}

#define IMPLEMENT_ARGS_PCT(VarName, AliasName, TextFmt, NextTextFmt) \
	{ \
		static const FTextFormat Fmt = TextFmt; \
		static const FTextFormat NextFmt = NextTextFmt; \
		const FText ValueText = FText::AsNumber(DescriptionInfo.VarName * 100.f, &Options); \
		const FText NextValueText = FText::AsNumber(DescriptionInfo.Next##VarName * 100.f, &Options); \
		InArgs.Add(TEXT(AliasName),  FText::FormatOrdered(Fmt, ValueText)); \
		const bool bEqual = FMath::IsNearlyEqual(DescriptionInfo.VarName, DescriptionInfo.Next##VarName, 0.05f); \
		InArgs.Add(TEXT("Next" AliasName), FText::FormatOrdered(bEqual ? Fmt : NextFmt, NextValueText, ValueText)); \
	}

#define IMPLEMENT_ARGS_INT(VarName, AliasName, TextFmt, NextTextFmt) \
	{ \
		static const FTextFormat Fmt = TextFmt; \
		static const FTextFormat NextFmt = NextTextFmt; \
		InArgs.Add(TEXT(AliasName),  FText::FormatOrdered(Fmt, DescriptionInfo.VarName)); \
		const bool bEqual = (DescriptionInfo.VarName == DescriptionInfo.Next##VarName); \
		InArgs.Add(TEXT("Next" AliasName), FText::FormatOrdered(bEqual ? Fmt : NextFmt, DescriptionInfo.Next##VarName, DescriptionInfo.VarName)); \
	}

void FDynamicDescriptionInfo::UpdateDescriptionPredefinedTextArgs(FFormatNamedArguments& InArgs, const UAuraGameplayAbility* Ability, const int32 InLevel)
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
		FText DamageText, NextDamageText;
		UpdateDescriptionTextArgs_Damage(InArgs, DescriptionInfo.DamageType, DescriptionInfo.Damage, DescriptionInfo.NextDamage, DamageText, NextDamageText);
		InArgs.FindOrAdd(TEXT("Damage")) = DamageText;
		InArgs.FindOrAdd( TEXT("NextDamage")) = NextDamageText;
	}

	if (DescriptionInfo.bManaCostText)
	{
		IMPLEMENT_ARGS(ManaCost, "Cost", INVTEXT("<mana>{0}</>"), INVTEXT("<old>{1}</> > <mana>{0}</>"));
	}

	if (DescriptionInfo.bCooldownText)
	{
		IMPLEMENT_ARGS(Cooldown, "Cooldown", INVTEXT("<Cooldown>{0}</>"), INVTEXT("<old>{1}</> > <Cooldown>{0}</>"));
	}

	if (DescriptionInfo.bDebuffText && DescriptionInfo.DebuffChance > 0.f)
	{
		UpdateDescriptionTextArgs_Debuff(InArgs, DescriptionInfo);
	}

	if (DescriptionInfo.bProjectileText)
	{
		IMPLEMENT_ARGS_INT(NumProjectiles, "Projectiles", INVTEXT("<num>{0}</>"), INVTEXT("<old>{1}</> > <num>{0}</>"));
		
		// we also need raw (not-formatted) numbers as variables to have the ability to use plural format
		InArgs.Add(TEXT("NumProjectiles"), DescriptionInfo.NumProjectiles);
		InArgs.Add(TEXT("NextNumProjectiles"), DescriptionInfo.NextNumProjectiles);
	}
}

void FDynamicDescriptionInfo::UpdateDescriptionTextArgs_Damage(FFormatNamedArguments& InArgs,
	const FGameplayTag& DamageType, const float Damage, const float NextDamage, FText& OutDamageText, FText& OutNextDamageText )
{
	static const FTextFormat GenericDamageTypeFmt = LOCTEXT("GenericDamageFmt", "<dmg>{0}</> damage");
	static const FTextFormat FireDamageTypeFmt = LOCTEXT("FireDamageFmt", "<dmgfire>{0} fire</> damage");
	static const FTextFormat LightningDamageTypeFmt = LOCTEXT("LightningDamageFmt", "<dmglght>{0} lightning</> damage");
	static const FTextFormat ArcaneDamageTypeFmt = LOCTEXT("ArcaneDamageFmt", "<dmgarc>{0} arcane</> damage");

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
	static const FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(1);
	
	const auto DamageText = FText::AsNumber(Damage, &Options);
	const auto NextDamageText = FText::AsNumber(NextDamage, &Options);
	OutDamageText = FText::Format(*DamageFmt, DamageText);
	OutNextDamageText = FText::Format(*NextDamageFmt, NextDamageText, DamageText);
}

void FDynamicDescriptionInfo::UpdateDescriptionTextArgs_Debuff(FFormatNamedArguments& InArgs, const FDynamicDescriptionInfo& DescriptionInfo)
{
	{
		FText DamageText, NextDamageText;
		UpdateDescriptionTextArgs_Damage(InArgs, DescriptionInfo.DamageType, DescriptionInfo.DebuffDamage, DescriptionInfo.NextDebuffDamage, DamageText, NextDamageText);
		InArgs.Add(TEXT("DebuffDamage"), DamageText);
		InArgs.Add(TEXT("NextDebuffDamage"), NextDamageText);
	}

	static const FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(1);

	IMPLEMENT_ARGS_PCT(DebuffChance, "DebuffChance", INVTEXT("<pct>{0}%</>"), INVTEXT("<old>{1}%</> > <pct>{0}%</>"));
	IMPLEMENT_ARGS(DebuffFrequency, "DebuffFreq", INVTEXT("<pct>{0}</>"), INVTEXT("<old>{1}</> > <pct>{0}</>"));
	IMPLEMENT_ARGS(DebuffDuration, "DebuffDuration", INVTEXT("<pct>{0}</>"), INVTEXT("<old>{1}</> > <pct>{0}</>"));
}

void FDynamicDescriptionInfo::AnalyzeDescription(TSubclassOf<UAuraGameplayAbility> AbilityClass)
{
	UAuraGameplayAbility* CDO = AbilityClass->GetDefaultObject<UAuraGameplayAbility>();
	if (CDO == nullptr || CDO->DynamicDescriptionInfo.bTextInitialized || CDO->Description.IsEmpty())
		return;

	const FString Desc = CDO->Description.ToString();
	const FString NextDesc = CDO->NextLevelDescription.ToString();

	CDO->DynamicDescriptionInfo.bDamageText = Desc.Contains(TEXT("{Damage}"), ESearchCase::IgnoreCase);
	CDO->DynamicDescriptionInfo.bManaCostText = Desc.Contains(TEXT("{Cost}"), ESearchCase::IgnoreCase);
	CDO->DynamicDescriptionInfo.bCooldownText = Desc.Contains(TEXT("{Cooldown}"), ESearchCase::IgnoreCase);
	CDO->DynamicDescriptionInfo.bProjectileText = Desc.Contains(TEXT("Projectiles}"), ESearchCase::IgnoreCase);
	CDO->DynamicDescriptionInfo.bDebuffText = Desc.Contains(TEXT("{Debuff"), ESearchCase::IgnoreCase);
#if !WITH_EDITOR
  	CDO->DescriptionInfo.bTextInitialized = true;
#endif
}

#undef LOCTEXT_NAMESPACE