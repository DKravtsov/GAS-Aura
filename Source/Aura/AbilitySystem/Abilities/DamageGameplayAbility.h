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

public:


	UFUNCTION(BlueprintCallable, Category = Caombat)
	void CauseDamageToActor(AActor* TargetActor);

	// Causes damage to the array of the actors, checking if they are not friendly.
	// Returns true if at least one damage was applied
	UFUNCTION(BlueprintCallable, Category = Caombat)
	bool CauseDamageToActors(const TArray<AActor*>& TargetActors);

	UFUNCTION(BlueprintPure)
	float GetBaseDamage(const int32 InLevel) const;

	UFUNCTION(BlueprintPure)
	FGameplayTag GetDamageType() const { return DamageType;}

	UFUNCTION(BlueprintPure)
    FDamageEffectParams MakeDamageEffectParams(AActor* TargetActor = nullptr) const;

protected:

    void SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle) const;

	virtual void GetDynamicDescriptionInfo(FDynamicDescriptionInfo& OutDescriptionInfo, const int32 InLevel) const override;
};
