// Copyright 4sandwiches


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(EAuraCharacterClass CharacterClass)
{
    return ClassInfo.FindChecked(CharacterClass);
}

float UCharacterClassInfo::GetDamageCalculationCoef(FName CurveName, int32 ApplyLevel) const
{
    constexpr float DefaultCoef = 1.f;

    if (ensure(DamageCalculationCoefs))
    {
        static const FString ContextString("GetDamageCalculationCoef");
        if (FRealCurve* ArmorPenetrationCurve = DamageCalculationCoefs->FindCurve(CurveName, ContextString))
        {
            return ArmorPenetrationCurve->Eval(ApplyLevel, DefaultCoef);
        }
    }
    return DefaultCoef;
}
