// Copyright 4sandwiches


#include "AbilitySystem/ModMagCalc/MMCMaxHealth.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Characters/CombatInterface.h"

UMMCMaxHealth::UMMCMaxHealth()
{
    VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
    VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
    VigorDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(VigorDef);
}

float UMMCMaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluatedParams;
    EvaluatedParams.SourceTags = SourceTags;
    EvaluatedParams.TargetTags = TargetTags;

    float Vigor = 0.f;

    if (GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluatedParams, Vigor))
    {
        const float VigorMinValue = 0.f;
        Vigor = FMath::Max(Vigor, VigorMinValue);
    }

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
    const int32 CharacterLevel = CombatInterface ? CombatInterface->GetCharacterLevel() : 1.f;
    
    return 80.f + 2.5f * Vigor + 10.f * CharacterLevel;
}
