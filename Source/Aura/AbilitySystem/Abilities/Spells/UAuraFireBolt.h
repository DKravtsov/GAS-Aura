// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "UAuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UUAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	int32 MaxNumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt", meta = (EditCondition="bUseHomingProjectiles"))
	FVector2D HomingAccelerationMinMax = {1600.f, 3200.f};

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt", meta = (InlineEditConditionToggle=true))
	uint8 bUseHomingProjectiles:1 = true;
	
public:

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, AActor* HomingTargetActor);

	int32 GetNumProjectiles(const int32 InLevel) const;
	
protected:
	
	virtual void GetDynamicDescriptionInfo(FDynamicDescriptionInfo& OutDescriptionInfo, const int32 InLevel) const override;
};
