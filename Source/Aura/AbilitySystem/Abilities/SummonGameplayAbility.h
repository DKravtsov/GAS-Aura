// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "SummonGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USummonGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly, Category="Summoning", BlueprintReadOnly)
	int32 NumMinions = 5;

	// Classes of Minions to spawn with weights
	UPROPERTY(EditDefaultsOnly, Category="Summoning", BlueprintReadOnly)
	TMap<TSubclassOf<APawn>, float> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	FVector2D MinMaxSpawnDistance = {50.f, 250.f};

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	float SpawnSpread = 90.f;

private:

	UPROPERTY()
	mutable TArray<TSubclassOf<APawn>> MinionClassesArray;
	mutable TArray<float> Weights;
	mutable bool bInitialized = false;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<FVector> GetSpawnLocations() const;

	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomMinionClass() const;
};
