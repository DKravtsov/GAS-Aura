// Copyright 4sandwiches


#include "AuraInvetoryItemFragments.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Aura.h"
#include "GameplayEffect.h"
#include "Characters/AuraCharacterBase.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

void FAuraInventoryPotionConsumeModifier::OnConsume(const APlayerController* PC) const
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

void FAuraInventoryEffectEquipModifier::Manifest()
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

void FAuraInventoryEffectEquipModifier::OnEquip(const APlayerController* PC) const
{
	if (!IsValueValid() || !IsValid(PC) || !EffectClass)
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

void FAuraInventoryEffectEquipModifier::OnUnequip(const APlayerController* PC) const
{
	if (!IsValueValid() || !IsValid(PC) || !EquippedEffectHandle.IsValid())
		return;

	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PC->GetCharacter()))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(EquippedEffectHandle, 1);
	}
}

void FAuraInventoryAttributeEquipModifier::OnEquip(const APlayerController* PC) const
{
	if (!IsValueValid() || !IsValid(PC) || !Attribute.IsValid())
		return;
	ACharacter* Character = PC->GetCharacter();
	
	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character))
	{
		EquippedEffectHandle = CreateAndApplyAttributeGameplayEffect(AbilitySystemComponent, Attribute, GetValue());
	}
}

void FAuraInventoryAttributeEquipModifier::OnUnequip(const APlayerController* PC) const
{
	if (!IsValueValid() || !IsValid(PC) || !EquippedEffectHandle.IsValid())
		return;

	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PC->GetCharacter()))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(EquippedEffectHandle, 1);
	}
}

FActiveGameplayEffectHandle FAuraInventoryAttributeEquipModifier::CreateAndApplyAttributeGameplayEffect(
	UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayAttribute& InAttribute, const float InMagnitude)
{
	check(IsValid(InAbilitySystemComponent));
	FGameplayEffectContextHandle EffectContext = InAbilitySystemComponent->MakeEffectContext();
	
	const FString EffectName = TEXT("GE_Equip_Attribute_") + InAttribute.GetName();
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), *EffectName);

	Effect->DurationPolicy = EGameplayEffectDurationType::Infinite;

	// FInheritedTagContainer GrantedTagsComp;
	// GrantedTagsComp.Added.AddTag(/* ??? */);
	// Effect->AddComponent<UTargetTagsGameplayEffectComponent>().SetAndApplyTargetTagChanges(GrantedTagsComp);

	Effect->StackingType = EGameplayEffectStackingType::AggregateByTarget;
	//Effect->StackLimitCount = -1;

	FGameplayModifierInfo& ModInfo = Effect->Modifiers.Add_GetRef(FGameplayModifierInfo());
	ModInfo.ModifierMagnitude = FScalableFloat(InMagnitude);
	ModInfo.ModifierOp = EGameplayModOp::Additive;
	ModInfo.Attribute = InAttribute;

	if (FGameplayEffectSpec* Spec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FActiveGameplayEffectHandle ActiveEffectHandle = InAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
		// add Gameplay Cue here ?
		return ActiveEffectHandle;
	}
	return {};
}

