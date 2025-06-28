// Copyright 4sandwiches


#include "InvetoryItemFragments.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Characters/AuraCharacterBase.h"

void FAuraPotionInventoryFragment::OnConsume(const APlayerController* PC) const
{
	if (PotionEffectClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is trying to consume an item with AuraPotionInventoryFragment [%s] where PotionEffectClass is not specified."),
			*GetNameSafe(PC), *GetFragmentTag().ToString());
		return;
	}

	// if (AAuraCharacterBase* Character = IsValid(PC) ? Cast<AAuraCharacterBase>(PC->GetCharacter()) : nullptr; IsValid(Character))
	// {
	// 	Character->ApplyGameplayEffectToSelf(PotionEffectClass, PotionLevel);
	// }

	if (!IsValid(PC))
		return;
	ACharacter* Character = PC->GetCharacter();
	
	if (auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character))
	{
		auto EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(Character);
		auto SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(PotionEffectClass, PotionLevel, EffectContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, SetByCallerTag, GetValue());
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}
