// Copyright 4sandwiches


#include "AuraInvetoryItemFragments.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Aura.h"
#include "GameplayEffect.h"
#include "Characters/AuraCharacterBase.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

void FAuraPotionInventoryFragment::OnConsume(const APlayerController* PC) const
{
	if (PotionEffectClass == nullptr)
	{
		UE_LOG(LogAura, Error, TEXT("%s is trying to consume an item with AuraPotionInventoryFragment [%s] where PotionEffectClass is not specified."),
			*GetNameSafe(PC), *GetFragmentTag().ToString());
		return;
	}

	if (!IsValid(PC))
		return;
	ACharacter* Character = PC->GetCharacter();
	
	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character))
	{
		auto EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(Character);
		const auto SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(PotionEffectClass, PotionLevel, EffectContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, SetByCallerTag, GetValue());
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void FAuraEffectEquipModifier::Manifest()
{
	FInventoryEquipModifier::Manifest();

	if (EffectClass == nullptr)
	{
		UE_LOG(LogAura, Error, TEXT("Equip item with AuraEffectEquipModifier [%s] expects EffectClass is specified."),
			*GetFragmentTag().ToString());
		return;
	}
	const UGameplayEffect* CDO = GetDefault<UGameplayEffect>(EffectClass);
	const bool bInfinite = CDO->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (!bInfinite)
	{
		UE_LOG(LogAura, Error, TEXT("Equip item with AuraEffectEquipModifier [%s] expects Infinite duration policy in its EffectClass [%s]. Otherwise it will work incorrectly."),
			*GetFragmentTag().ToString(), *EffectClass->GetName());
		return;
	}
}

void FAuraEffectEquipModifier::OnEquip(const APlayerController* PC) const
{
	if (!IsValid(PC) || !EffectClass)
		return;
	ACharacter* Character = PC->GetCharacter();
	
	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character))
	{
		auto EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(Character);
		const auto SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, EquipLevel, EffectContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, SetByCallerTag, GetValue());
		EquippedEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void FAuraEffectEquipModifier::OnUnequip(const APlayerController* PC) const
{
	if (!IsValid(PC) || !EquippedEffectHandle.IsValid())
		return;

	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PC->GetCharacter()))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(EquippedEffectHandle, 1);
	}
}
