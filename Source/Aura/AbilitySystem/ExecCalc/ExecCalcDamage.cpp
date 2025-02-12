// Copyright 4sandwiches


#include "AbilitySystem/ExecCalc/ExecCalcDamage.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"

#include "DebugHelper.h"
#define PRINT_DEBUG1(V, T) Debug::Print(FString::Printf(TEXT("%s: %s"), TEXT(T), *FString::SanitizeFloat(V)))
#define PRINT_DEBUG(V) PRINT_DEBUG1(V, #V)


struct FAuraDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
    DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

    FAuraDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, IncomingDamage, Target, false);
    }

};

static const FAuraDamageStatics& DamageStatics()
{
    static FAuraDamageStatics DamageStatics_;

    return DamageStatics_;
}

UExecCalcDamage::UExecCalcDamage()
{
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
}

void UExecCalcDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
    //EffectSpec.GetContext().GetSourceObject();
    //EffectSpec.GetContext().GetAbility();
    //EffectSpec.GetContext().GetInstigator();
    //EffectSpec.GetContext().GetEffectCauser();

    FAggregatorEvaluateParameters Params;
    Params.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
    Params.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    float BaseDamage = EffectSpec.GetSetByCallerMagnitude(AuraGameplayTags::SetByCaller_BaseDamage, true);
    PRINT_DEBUG(BaseDamage);

    float BlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, Params, BlockChance);
    BlockChance = FMath::Max(0.f, BlockChance);
    PRINT_DEBUG(BlockChance);
    const bool bSuccessfulBlock = UKismetMathLibrary::RandomBoolWithWeight(BlockChance);
    PRINT_DEBUG1((bSuccessfulBlock ? 1 : 0), "SuccessfulBlock");

    // if successful block, cut the damage in half
    BaseDamage = bSuccessfulBlock ? (BaseDamage / 2) : BaseDamage;

    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, Params, TargetArmor);
    TargetArmor = FMath::Max(0.f, TargetArmor);
    PRINT_DEBUG(TargetArmor);

    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, Params, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max(0.f, SourceArmorPenetration);
    PRINT_DEBUG(SourceArmorPenetration);

    const float EffectiveArmor = FMath::Clamp(TargetArmor - SourceArmorPenetration, 0.f, 100.f);
    PRINT_DEBUG(EffectiveArmor);

    float FinalDamage = BaseDamage * (100.f - EffectiveArmor) / 100.f;
    PRINT_DEBUG(FinalDamage);
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingDamageProperty, EGameplayModOp::Override, FinalDamage));
}
