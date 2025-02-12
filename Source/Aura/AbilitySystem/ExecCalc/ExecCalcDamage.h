// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalcDamage.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UExecCalcDamage : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:

    UExecCalcDamage();

    void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
};
