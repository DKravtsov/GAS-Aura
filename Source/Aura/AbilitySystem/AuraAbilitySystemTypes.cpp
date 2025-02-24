// Copyright 4sandwiches

#include "AuraAbilitySystemTypes.h"

void FAuraGameplayEffectContext::SetupDebuffFromDamageParams(const FDamageEffectParams& Params)
{
    SetupDebuff(Params.DebuffDamage, Params.DebuffDuration, Params.DebuffFrequency);
}

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    Super::NetSerialize(Ar, Map, bOutSuccess);

    uint8 RepBits = 0;
    if (Ar.IsSaving())
    {
        RepBits |= bBlockedHit << 0;
        RepBits |= bCriticalHit << 1;
        RepBits |= bDebuff << 2;
        if (DebuffDamage > 0.f)
        {
            RepBits |= 1 << 3;
        }
        if (DebuffDuration > 0.f)
        {
            RepBits |= 1 << 4;
        }
        if (DebuffFrequency > 0.f)
        {
            RepBits |= 1 << 5;
        }
        if (DamageType.IsValid())
        {
            RepBits |= 1 << 6;
        }

        Ar.SerializeBits(&RepBits, 7);
    }
    else
    {
        Ar.SerializeBits(&RepBits, 7);

        bBlockedHit = RepBits & (1 << 0);
        bCriticalHit = RepBits & (1 << 1);
        bDebuff = RepBits & (1 << 2);
    }

    if (RepBits & (1 << 3))
    {
        Ar << DebuffDamage;
    }
    if (RepBits & (1 << 4))
    {
        Ar << DebuffDuration;
    }
    if (RepBits & (1 << 5))
    {
        Ar << DebuffFrequency;
    }
    if (RepBits & (1 << 6))
    {
        DamageType.NetSerialize(Ar, Map, bOutSuccess);
    }
    
    bOutSuccess = true;
    return true;
}
