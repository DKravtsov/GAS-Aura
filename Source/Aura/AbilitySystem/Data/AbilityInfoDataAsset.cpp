// Copyright 4sandwiches


#include "AbilitySystem/Data/AbilityInfoDataAsset.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

FAuraAbilityInfo UAbilityInfoDataAsset::FindAbilityInfoByTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	if (const auto Info = AbilityInfos.FindByKey(Tag))
	{
		return FAuraAbilityInfo(*Info, true);
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find any info for the AbilityTag [%s] on the AbilityInfo Data Asset [%s]"),
			*Tag.ToString(), *GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}
