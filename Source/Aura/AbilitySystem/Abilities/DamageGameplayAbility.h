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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (Categories="GameplayCue.Debuf"))
	FGameplayTag DebuffGameplayCueTag;

public:


	UFUNCTION(BlueprintCallable, Category = Caombat)
	void CauseDamageToActor(AActor* TargetActor);

	// Causes damage to the array of the actors, checking if they are not friendly.
	// Returns true if at least one damage was applied
	UFUNCTION(BlueprintCallable, Category = Caombat)
	bool CauseDamageToActors(const TArray<AActor*>& TargetActors);

	float GetBaseDamage(const int32 InLevel) const;
	FGameplayTag GetDamageType() const { return DamageType;}

    FDamageEffectParams MakeDamageEffectParams(AActor* TargetActor = nullptr) const;

protected:

    void SetupDamageTypes(const FGameplayEffectSpecHandle& DamageEffectSpecHandle) const;

	virtual void GetDynamicDescriptionInfo(FDynamicDescriptionInfo& OutDescriptionInfo, const int32 InLevel) const override;
};
