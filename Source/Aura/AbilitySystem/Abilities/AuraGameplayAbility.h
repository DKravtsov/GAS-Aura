// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AuraGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EAuraAbilityActivationPolicy : uint8
{
    OnTriggered,
    OnGiven,
};


/**
 *
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, Category = AuraAbility, meta = (Categories = "InputTag"))
    FGameplayTag StartupInputTag;

    UPROPERTY(EditDefaultsOnly, Category = AuraAbility)
    EAuraAbilityActivationPolicy ActivationPolicy = EAuraAbilityActivationPolicy::OnTriggered;

#pragma region Description properties
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description")
    FText AbilityName;

    /*
     * @brief: Description of the ability (spell)
     * Full description can use styles from the RichTextStyles data table;
     * You can use "variables" in curly brackets which will be replaced with the correct values, such as:
     *	Title
     *	Level
     *	Damage
     *	Cooldown
     *	...
     *	etc.
     *	For example, string "{Damage}" will be replaced with the value of damage for this level of the spell
     *	using 'dmg' style: "<dmg>25</>"
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description", meta = (MultiLine = true))
    FText Description;

    /*
     * @brief Description of the next level of the ability (spell)
     * Full description can use styles from the RichTextStyles data table;
     * You can use "variables" in curly brackets which will be replaced with the correct values, such as:
     *	NextLevel
     *	NextDamage
     *	NextCooldown
     *	...
     *	etc.
     *	or any other "variable" from the Description property
     *	For example, string "{NextDamage}" will be replaced with the value of damage for this level and next level
     *	of the spell using corresponding styles: "<old>25</> -> <dmg>28</>"
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description", meta = (MultiLine = true))
    FText NextLevelDescription;

#pragma endregion
    
public:

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    virtual void GetDescription(const int32 Level, FText& OutDesc, FText& OutNextLevelDesc) const;

    float GetManaCost(const int32 InLevel) const;
    float GetCooldown(const int32 InLevel) const;
    
protected:

#pragma region Description Internal
    
    struct FDynamicDescriptionInfo
    {
        uint32 bTextInitialized : 1 = false;

        uint32 bDamageText : 1 = false;
        uint32 bManaCostText : 1 = false;
        uint32 bCooldownText : 1 = false;

        float Damage = 0.f;
        float NextDamage = 0.f;
        FGameplayTag DamageType;

        float ManaCost = 0.f;
        float NextManaCost = 0.f;

        float Cooldown = 0.f;
        float NextCooldown = 0.f;
        
        static void AnalyzeDescription(TSubclassOf<UAuraGameplayAbility> AbilityClass);
        static void UpdateDescriptionPredefinedTextArgs(FFormatNamedArguments& InArgs, const UAuraGameplayAbility* Ability, const int32 InLevel);
    };

    virtual void GetDynamicDescriptionInfo(FDynamicDescriptionInfo& OutDescriptionInfo, const int32 InLevel) const;

    FDynamicDescriptionInfo DynamicDescriptionInfo;

#pragma endregion 
};
