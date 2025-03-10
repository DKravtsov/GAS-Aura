// Copyright 4sandwiches

#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilitySystemTypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

    UPROPERTY(BlueprintReadWrite)
    float BaseDamage = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float AbilityLevel = 1.f;

    UPROPERTY(BlueprintReadWrite)
    FGameplayTag DamageType;

    UPROPERTY(BlueprintReadWrite)
    float DebuffChance = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float DebuffDamage = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float DebuffFrequency = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float DebuffDuration = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float KnockBackChance;

    UPROPERTY(BlueprintReadWrite)
    float KnockBackImpulseMagnitude = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float DeathImpulseMagnitude = 0.f;

    UPROPERTY(BlueprintReadWrite)
    FVector DeathImpulse = FVector(ForceInitToZero);

    UPROPERTY(BlueprintReadWrite)
    FVector KnockBackImpulse = FVector(ForceInitToZero);

protected:
    TSharedPtr<FHitResult> HitResult;

public:

    bool HasHitResult() const { return HitResult != nullptr; }
    bool GetHitResult(FHitResult& OutHitResult) const;
    TSharedPtr<FHitResult> GetHitResult() const { return HitResult; }
    void SetHitResult(const FHitResult& InHitResult);
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

protected:

    UPROPERTY()
    uint8 bBlockedHit:1 = false;

    UPROPERTY()
    uint8 bCriticalHit:1 = false;

    UPROPERTY()
    uint8 bDebuff:1 = false;
    
    UPROPERTY()
    float DebuffDamage = 0.f;

    UPROPERTY()
    float DebuffDuration = 0.f;

    UPROPERTY()
    float DebuffFrequency = 0.f;

    UPROPERTY()
    FGameplayTag DamageType;

    UPROPERTY()
    FVector DeathImpulse = FVector(ForceInitToZero);

    UPROPERTY()
    FVector KnockBackImpulse = FVector(ForceInitToZero);

public:

    FAuraGameplayEffectContext()
    {
    }
    FAuraGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
        : FGameplayEffectContext(InInstigator, InEffectCauser)
    {
    }

    bool IsCriticalHit() const { return bCriticalHit; }
    bool IsBlockedHit() const { return bBlockedHit; }

    void SetIsCriticalHit(bool bNewValue) { bCriticalHit = bNewValue; }
    void SetIsBlockedHit(bool bNewValue) { bBlockedHit = bNewValue; }

    bool IsSuccessfulDebuff() const { return bDebuff; }
    void SetIsSuccessfulDebuff(bool bNewValue) { bDebuff = bNewValue; }

    float GetDebuffDamage() const { return DebuffDamage; }
    float GetDebuffDuration() const { return DebuffDuration; }
    float GetDebuffFrequency() const { return DebuffFrequency; }
    void SetupDebuff(const float InDamage, const float InDuration, const float InFrequency);
    void SetupDebuffFromDamageParams(const FDamageEffectParams& Params);

    const FGameplayTag& GetDamageType() const { return DamageType; }
    void SetDamageType(const FGameplayTag& InDamageType) { DamageType = InDamageType; }

    FVector GetDeathImpulse() const { return DeathImpulse; }
    void SetDeathImpulse(const FVector& ImpulseVector) { DeathImpulse = ImpulseVector; }

    FVector GetKnockBackImpulse() const { return  KnockBackImpulse; }
    void SetKnockBackImpulse(const FVector& ImpulseVector) { KnockBackImpulse = ImpulseVector; }

    virtual UScriptStruct* GetScriptStruct() const override
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

FORCEINLINE void FAuraGameplayEffectContext::SetupDebuff(const float InDamage, const float InDuration, const float InFrequency)
{
    bDebuff = true;
    DebuffDamage = InDamage;
    DebuffDuration = InDuration;
    DebuffFrequency = InFrequency;
}