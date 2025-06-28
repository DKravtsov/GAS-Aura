// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraPassiveSkill.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraPassiveSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAuraAbilitySystemComponent* ASC = GetAuraAbilitySystemComponent())
	{
		ASC->OnPassiveAbilityDeactivated.AddUObject(this, &UAuraPassiveSkill::ReceiveDeactivate);
	}
}

void UAuraPassiveSkill::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (GetAssetTags().HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
