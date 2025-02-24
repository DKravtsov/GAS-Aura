// Copyright 4sandwiches

#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilitySystemTypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
    GENERATED_BODY()

    UPROPERTY()
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

    UPROPERTY()
    float BaseDamage = 0.f;

    UPROPERTY()
    float AbilityLevel = 1.f;

    UPROPERTY()
    FGameplayTag DamageType;

    UPROPERTY()
    float DebuffChance = 0.f;
    
    UPROPERTY()
    float DebuffDamage = 0.f;
    
    UPROPERTY()
    float DebuffFrequency = 0.f;
    
    UPROPERTY()
    float DebuffDuration = 0.f;

};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

protected:

    UPROPERTY()
    uint8 bBlockedHit : 1;

    UPROPERTY()
    uint8 bCriticalHit : 1;

public:

    FAuraGameplayEffectContext()
        : bBlockedHit(false)
        , bCriticalHit(false)
    {
    }
    FAuraGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
        : FGameplayEffectContext(InInstigator, InEffectCauser)
        , bBlockedHit(false)
        , bCriticalHit(false)
    {
    }

    bool IsCriticalHit() const { return bCriticalHit; }
    bool IsBlockedHit() const { return bBlockedHit; }

    void SetIsCriticalHit(bool bNewValue) { bCriticalHit = bNewValue; }
    void SetIsBlockedHit(bool bNewValue) { bBlockedHit = bNewValue; }

    UScriptStruct* GetScriptStruct() const override
    {
        return StaticStruct();
    }

    /** Creates a copy of this context, used to duplicate for later modifications */
    virtual FGameplayEffectContext* Duplicate() const override
    {
        FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }

    virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
};

template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
    enum
    {
        WithNetSerializer = true,
        WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
    };
};
