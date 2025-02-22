// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

UENUM()
enum class EAbilityDamagePolicy : uint8
{
    NoDamage,
    CausesDamage,
};

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

    UPROPERTY(EditDefaultsOnly, Category = AuraAbility)
    EAbilityDamagePolicy DamagePolicy = EAbilityDamagePolicy::NoDamage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage, meta = (EditCondition = "DamagePolicy==EAbilityDamagePolicy::CausesDamage", EditConditionHides))
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage, meta = (EditCondition = "DamagePolicy==EAbilityDamagePolicy::CausesDamage", EditConditionHides, Categories="Damage", ForceInlineRow))
    TMap<FGameplayTag, FScalableFloat> DamageTypes;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description")
    FText AbilityName;

    /*
     * @brief: Description of the ability (spell)
     * Full description can use styles from the RichTextStyles data table;
     * Alse can be used "variables" in curly brackets, such as:
     *	Title
     *	Level
     *	Damage
     *	Cooldown
     *	...
     *	etc.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description", meta = (MultiLine = true))
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description", meta = (MultiLine = true))
    FText NextLevelDescription;
    
public:

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION(BlueprintCallable, Category = Caombat)
    void CauseDamageToActor(AActor* TargetActor);

    // Causes damage to the array of the actors, checking if they are not friendly.
    // Returns true if at least one damage was applied
    UFUNCTION(BlueprintCallable, Category = Caombat)
    bool CauseDamageToActors(const TArray<AActor*>& TargetActors);

    virtual void GetDescription(const int32 Level, FText& OutDesc, FText& OutNextLevelDesc) const;

protected:

    void SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle);

    float CalculateTotalDamage(int32 InLevel) const;
    float GetDamageByType(const FGameplayTag& DamageType, const int32 InLevel) const;
    float GetManaCost(const int32 InLevel) const;
    float GetCooldown(const int32 InLevel) const;
    
    virtual void UpdateDescriptionTextArgs(FFormatNamedArguments& InArgs, const int32 InLevel) const;
    virtual void UpdateDescriptionPredefinedTextArgs(FFormatNamedArguments& InArgs, const int32 InLevel) const;

    virtual void AnalyzeDescription();

protected:
    uint32 bDamageText : 1 = false;
    uint32 bDamageTypeFireText : 1 = false;
    uint32 bDamageTypeLightningText : 1 = false;
    uint32 bDamageTypeArcaneText : 1 = false;
    uint32 bManaCostText : 1 = false;
    uint32 bCooldownText : 1 = false;
    uint32 bTextInitialized : 1 = false;  
};
