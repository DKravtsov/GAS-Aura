// Copyright 4sandwiches


#include "InvetoryItemFragments.h"

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

	if (AAuraCharacterBase* Character = IsValid(PC) ? Cast<AAuraCharacterBase>(PC->GetCharacter()) : nullptr; IsValid(Character))
	{
		Character->ApplyGameplayEffectToSelf(PotionEffectClass, PotionLevel);
	}
}
