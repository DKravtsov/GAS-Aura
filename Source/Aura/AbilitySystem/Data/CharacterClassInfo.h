// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

UENUM(BlueprintType)
enum class EAuraCharacterClass : uint8
{
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

    UPROPERTY(EditDefaultsOnly, Category = "Common Class Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> Abilities;

public:

    FCharacterClassDefaultInfo GetClassDefaultInfo(EAuraCharacterClass CharacterClass);
};
