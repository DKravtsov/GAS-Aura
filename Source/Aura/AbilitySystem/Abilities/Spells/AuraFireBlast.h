// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UDamageGameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FireBlast")
	int32 NumFireBalls = 12;
};
