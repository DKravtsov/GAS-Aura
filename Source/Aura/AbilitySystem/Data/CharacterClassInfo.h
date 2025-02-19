// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

UENUM(BlueprintType)
enum class EAuraCharacterClass : uint8
{
    Undefined,
    Warrior,
    Ranger,
    Elementalist,

    MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
    TSubclassOf<class UGameplayEffect> PrimaryAttributes;

    UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
    TArray<TSubclassOf<class UGameplayAbility>> Abilities;

    UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
    TObjectPtr<class UBehaviorTree> ClassBehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
    FScalableFloat RewardXP;
};

/**
 *
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, Category = "Class Defaults", meta = (ForceInlineRow))
    TMap<EAuraCharacterClass, FCharacterClassDefaultInfo> ClassInfo;

    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
    TSubclassOf<class UGameplayEffect> SecondaryAttributes;

    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
    TSubclassOf<class UGameplayEffect> VitalAttributes;

    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
    TArray<TSubclassOf<class UGameplayAbility>> Abilities;

    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults|Damage")
    TObjectPtr<class UCurveTable> DamageCalculationCoefs;

public:

    const FCharacterClassDefaultInfo& GetClassDefaultInfo(EAuraCharacterClass CharacterClass);

    float GetDamageCalculationCoef(FName CurveName, int32 ApplyLevel) const;
};
