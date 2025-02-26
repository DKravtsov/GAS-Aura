// Copyright 4sandwiches

#pragma once
#include "GameplayTagContainer.h"

struct FDynamicDescriptionInfo
{
	uint32 bTextInitialized : 1 = false;

	uint32 bDamageText : 1 = false;
	uint32 bManaCostText : 1 = false;
	uint32 bCooldownText : 1 = false;
	uint32 bProjectileText:1 = false;
	uint32 bDebuffText:1 = false;

	float Damage = 0.f;
	float NextDamage = 0.f;
	FGameplayTag DamageType;

	float ManaCost = 0.f;
	float NextManaCost = 0.f;

	float Cooldown = 0.f;
	float NextCooldown = 0.f;

	int32 NumProjectiles = 0;
	int32 NextNumProjectiles = 0;

	float DebuffChance = 0.f;
	float DebuffDamage = 0.f;
	float DebuffFrequency = 0.f;
	float DebuffDuration = 0.f;
	float NextDebuffChance = 0.f;
	float NextDebuffDamage = 0.f;
	float NextDebuffFrequency = 0.f;
	float NextDebuffDuration = 0.f;
        
	static void AnalyzeDescription(TSubclassOf<class UAuraGameplayAbility> AbilityClass);
	static void UpdateDescriptionPredefinedTextArgs(FFormatNamedArguments& InArgs, const UAuraGameplayAbility* Ability, const int32 InLevel);

private:
	static void UpdateDescriptionTextArgs_Damage(FFormatNamedArguments& InArgs, const FGameplayTag& DamageType, const float Damage, const float NextDamage, FText& OutDamageText, FText& OutNextDamageText);
	static void UpdateDescriptionTextArgs_Debuff(FFormatNamedArguments& InArgs, const FDynamicDescriptionInfo& DescriptionInfo);
};
