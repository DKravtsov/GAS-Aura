// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraPassiveSkill.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraPassiveSkill : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:

	//~ Begin of UGameplayAbility interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	//~ End of UGameplayAbility interface

	void ReceiveDeactivate(const FGameplayTag& AbilityInfo);
};
