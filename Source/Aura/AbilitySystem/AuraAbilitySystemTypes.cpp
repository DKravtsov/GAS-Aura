// Copyright 4sandwiches

#include "AuraAbilitySystemTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    Super::NetSerialize(Ar, Map, bOutSuccess);

    uint8 RepBits = 0;
    if (Ar.IsSaving())
    {
        RepBits |= bBlockedHit << 0;
        RepBits |= bCriticalHit << 1;

        Ar.SerializeBits(&RepBits, 2);
    }
    else
    {
        Ar.SerializeBits(&RepBits, 2);

        bBlockedHit = RepBits & 1;
        bCriticalHit = RepBits & 2;
    }
    bOutSuccess = true;
    return true;
}
