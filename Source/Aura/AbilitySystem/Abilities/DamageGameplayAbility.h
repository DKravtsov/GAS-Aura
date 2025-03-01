// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "DamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (Categories="Damage"))
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Debuff")
	FScalableFloat DebuffChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Debuff")
	FScalableFloat DebuffDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Debuff")
	FScalableFloat DebuffFrequency;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Debuff")
	FScalableFloat DebuffDuration;

	// #todo: implementing this rather than hardcoded values needs to pass the tag through EffectContext in the current system.
	// I need to decide if this worth it
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Debuff", meta = (Categories="GameplayCue.Debuf"))
	//FGameplayTag DebuffGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float KnockBackChance = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (EditCondition="bKnockBackPitchOverride"))
	float KnockBackPitch = 45.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float KnockBackImpulseMagnitude = 600.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DeathImpulseMagnitude = 6000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (InlineEditConditionToggle="true"))
	uint8 bKnockBackPitchOverride:1 = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|RadialDamage")
	uint8 bRadialDamage:1 = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|RadialDamage", meta = (EditCondition="bRadialDamage"))
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|RadialDamage", meta = (EditCondition="bRadialDamage"))
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|RadialDamage", meta = (EditCondition="bRadialDamage"))
	float RadialDamageFalloff = 1.f;

public:


	UFUNCTION(BlueprintCallable, Category = "Combat", BlueprintPure = false)
	void CauseDamageToActor(AActor* TargetActor) const;

	// Causes damage to the array of the actors, checking if they are not friendly.
	// Returns true if at least one damage was applied
	UFUNCTION(BlueprintCallable, Category = "Combat", BlueprintPure = false)
	bool CauseDamageToActors(const TArray<AActor*>& TargetActors) const;

	UFUNCTION(BlueprintCallable, Category = "Combat", BlueprintPure = false, meta=(ReturnDisplayName="EffectContexts", AutoCreateRefTerm="IgnoreActors"))
	void CauseDamageToActorsInRadius(const FVector& Origin, const TArray<AActor*>& IgnoreActors) const;

	UFUNCTION(BlueprintPure)
	float GetBaseDamage(const int32 InLevel) const;

	UFUNCTION(BlueprintPure)
	FGameplayTag GetDamageType() const { return DamageType;}

	UFUNCTION(BlueprintPure)
    FDamageEffectParams MakeDamageEffectParams(AActor* TargetActor = nullptr) const;

protected:

    void SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle) const;

	virtual void GetDynamicDescriptionInfo(FDynamicDescriptionInfo& OutDescriptionInfo, const int32 InLevel) const override;
	
	void AddKnockBackParams(const AActor* TargetActor, const FVector& Origin, FDamageEffectParams& Params) const;
};
