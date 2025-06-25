// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "UObject/Object.h"
#include "InvetoryItemFragments.generated.h"

USTRUCT(BlueprintType)
struct FAuraPotionInventoryFragment: public FInventoryItemConsumableFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<class UGameplayEffect> PotionEffectClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float PotionLevel;

	virtual void OnConsume(const APlayerController* PC) override;
};
