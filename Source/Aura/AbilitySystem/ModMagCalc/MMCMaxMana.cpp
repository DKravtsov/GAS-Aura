// Copyright 4sandwiches


#include "AbilitySystem/ModMagCalc/MMCMaxMana.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Characters/CombatInterface.h"

UMMCMaxMana::UMMCMaxMana()
{
    IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
    IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
    IntelligenceDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMCMaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluatedParams;
    EvaluatedParams.SourceTags = SourceTags;
    EvaluatedParams.TargetTags = TargetTags;

    float Intelligence = 0.f;

    if (GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluatedParams, Intelligence))
    {
        const float IntelligenceMinValue = 0.f;
        Intelligence = FMath::Max(Intelligence, IntelligenceMinValue);
    }

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
    const int32 CharacterLevel = CombatInterface ? CombatInterface->GetCharacterLevel() : 1.f;

    return 50.f + 2.f * Intelligence + 5.f * CharacterLevel;
}
