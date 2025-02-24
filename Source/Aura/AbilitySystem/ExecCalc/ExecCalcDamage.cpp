// Copyright 4sandwiches


#include "AbilitySystem/ExecCalc/ExecCalcDamage.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "Game/AuraBlueprintFunctionLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/CurveTable.h"
#include "Characters/CombatInterface.h"

#include "DebugHelper.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#define PRINT_DEBUG1(V, T) //Debug::Print(FString::Printf(TEXT("%s: %s"), TEXT(T), *FString::SanitizeFloat(V)))
#define PRINT_DEBUG(V) PRINT_DEBUG1(V, #V)


struct FAuraDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
    DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritHitChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritHitDamage);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritHitResistance);
    
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);

    TArray<FGameplayTag> DamageTypes;
    
    FAuraDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, IncomingDamage, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritHitChance, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritHitDamage, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritHitResistance, Target, false);
        
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);

        DamageTypes = FGameplayTagHelper::RequestAllDamageTypeGameplayTags().GetGameplayTagArray();
    }

    const FGameplayEffectAttributeCaptureDefinition& GetResistanceDefByDamageType(const FGameplayTag& DamageType) const
    {
        if (DamageType.MatchesTagExact(AuraGameplayTags::Damage_Arcane)) return ArcaneResistanceDef;
        if (DamageType.MatchesTagExact(AuraGameplayTags::Damage_Physical)) return PhysicalResistanceDef;
        if (DamageType.MatchesTagExact(AuraGameplayTags::Damage_Fire)) return FireResistanceDef;
        if (DamageType.MatchesTagExact(AuraGameplayTags::Damage_Lightning)) return LightningResistanceDef;

        checkNoEntry();
        static FGameplayEffectAttributeCaptureDefinition NeverReturned;
        return NeverReturned;
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
    RelevantAttributesToCapture.Add(DamageStatics().CritHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CritHitDamageDef);
    RelevantAttributesToCapture.Add(DamageStatics().CritHitResistanceDef);
    
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
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

    FGameplayEffectContextHandle EffectContextHandle = EffectSpec.GetContext();

    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    const int32 SourceCharacterLevel = UAuraBlueprintFunctionLibrary::GetCharacterLevel(SourceAvatar);
    const int32 TargetCharacterLevel = UAuraBlueprintFunctionLibrary::GetCharacterLevel(TargetAvatar);

    const UCharacterClassInfo* CharacterClassInfo = UAuraBlueprintFunctionLibrary::GetCharacterClassInfo(TargetASC);

    float BaseDamage = 0.f;
    for (const auto& DamageType : DamageStatics().DamageTypes)
    {
        float Damage = EffectSpec.GetSetByCallerMagnitude(DamageType, false, 0.f);
        if (Damage > 0.f)
        {
            const auto& ResistanceDef = DamageStatics().GetResistanceDefByDamageType(DamageType);

            float Resistance = 0.f;
            ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceDef, Params, Resistance);
            // do not clamp resist value: it can be negative or more than 1

            // debuff
            const float DebuffChance = EffectSpec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_SetByCaller_Chance, false, -1);
            const float EffectiveDebuffChance = FMath::Clamp(DebuffChance * (1.f - Resistance), 0.f, 1.f);
            PRINT_DEBUG(EffectiveDebuffChance);
            const bool bDebuffTriggered = UKismetMathLibrary::RandomBoolWithWeight(EffectiveDebuffChance);
            PRINT_DEBUG1((bDebuffTriggered ? 1 : 0), "DebuffTriggered");
            if (bDebuffTriggered)
            {
                FGameplayEffectContextHandle ContextHandle = EffectSpec.GetContext();

                const float DebuffDamage = EffectSpec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_SetByCaller_Damage, false);
                const float DebuffDuration = EffectSpec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_SetByCaller_Duration, false);
                const float DebuffFrequency = EffectSpec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_SetByCaller_Frequency, false);

                UAuraBlueprintFunctionLibrary::SetupDebuffParams(ContextHandle, DebuffDamage, DebuffDuration, DebuffFrequency, DamageType);
            }

            // damage
            Damage *= (1.f - Resistance);
            BaseDamage += FMath::Max(Damage, 0.f);
        }
    }
    PRINT_DEBUG(BaseDamage);

    float CritChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritHitChanceDef, Params, CritChance);
    //CritChance = FMath::Clamp(CritChance, 0.f, 1.f);
    PRINT_DEBUG(CritChance);

    float CritResistance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritHitResistanceDef, Params, CritResistance);
    //CritResistance = FMath::Clamp(CritResistance, 0.f, 1.f);
    PRINT_DEBUG(CritResistance);

    const float CritResistanceCoef = CharacterClassInfo->GetDamageCalculationCoef("CritHitResistance", TargetCharacterLevel);
    PRINT_DEBUG(CritResistanceCoef);

    const float EffectiveCritChance = FMath::Clamp(CritChance - CritResistance * CritResistanceCoef, 0.f, 1.f);
    PRINT_DEBUG(EffectiveCritChance);
    const bool bCritHit = UKismetMathLibrary::RandomBoolWithWeight(EffectiveCritChance);
    PRINT_DEBUG1((bCritHit ? 1 : 0), "CritHit");

    if (bCritHit)
    {
        float CritDamage = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritHitDamageDef, Params, CritDamage);
        CritDamage = FMath::Max(CritDamage, 0.f);
        PRINT_DEBUG(CritDamage);

        const float CritDamageCoef = CharacterClassInfo->GetDamageCalculationCoef("CritHitDamage", SourceCharacterLevel);
        
        BaseDamage = 2 * BaseDamage + CritDamage * CritDamageCoef;

        UAuraBlueprintFunctionLibrary::SetIsCriticalHit(EffectContextHandle, true);
    }

    float BlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, Params, BlockChance);
    BlockChance = FMath::Max(0.f, BlockChance);
    PRINT_DEBUG(BlockChance);
    const bool bSuccessfulBlock = !bCritHit && UKismetMathLibrary::RandomBoolWithWeight(BlockChance);
    UAuraBlueprintFunctionLibrary::SetIsBlockedHit(EffectContextHandle, bSuccessfulBlock);
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

    const float ArmorPenetrationCoef = CharacterClassInfo->GetDamageCalculationCoef("ArmorPenetration", SourceCharacterLevel);

    const float EffectiveArmor = FMath::Clamp(TargetArmor - SourceArmorPenetration * ArmorPenetrationCoef, 0.f, 100.f);
    PRINT_DEBUG(EffectiveArmor);

    const float EffectiveArmorCoef = CharacterClassInfo->GetDamageCalculationCoef("EffectiveArmor", TargetCharacterLevel);
    const float FinalDamage = BaseDamage * (100.f - EffectiveArmor * EffectiveArmorCoef) / 100.f;
    PRINT_DEBUG(FinalDamage);
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingDamageProperty, EGameplayModOp::Override, FinalDamage));
}
