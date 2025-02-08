// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMCMaxMana.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UMMCMaxMana : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:

    UMMCMaxMana();

    float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

    FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};
