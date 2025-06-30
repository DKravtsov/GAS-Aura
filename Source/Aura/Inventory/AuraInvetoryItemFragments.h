// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "ScalableFloat.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "AuraInvetoryItemFragments.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAuraInventoryPotionConsumeModifier: public FInventoryConsumeModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayEffect> PotionEffectClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float PotionLevel = 1.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag SetByCallerTag;

	virtual void OnConsume(const APlayerController* PC) const override;
};

USTRUCT(BlueprintType)
struct FInventoryScalableFloat : public FInventoryNumericValueBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FScalableFloat Value{0.f};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Level = 1.f;

	virtual float GetValue() const override {return Value.GetValueAtLevel(GetLevel());}

	float GetLevel() const {return Level;}
	void SetLevel(const float NewLevel) {Level = NewLevel;}
};

USTRUCT(BlueprintType)
struct FAuraInventoryEffectEquipModifier : public FInventoryEquipModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float EquipLevel = 1.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag SetByCallerTag;

private:

	mutable FActiveGameplayEffectHandle EquippedEffectHandle;

public:

	virtual void Manifest() override;

	virtual void OnEquip(const APlayerController* PC) const override;
	virtual void OnUnequip(const APlayerController* PC) const override;

};

USTRUCT(BlueprintType)
struct FAuraInventoryAttributeEquipModifier : public FInventoryEquipModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayAttribute Attribute;

private:

	mutable FActiveGameplayEffectHandle EquippedEffectHandle;

public:

	virtual void OnEquip(const APlayerController* PC) const override;
	virtual void OnUnequip(const APlayerController* PC) const override;

private:
	
	static FActiveGameplayEffectHandle CreateAndApplyAttributeGameplayEffect(
		UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayAttribute& InAttribute,  const float InMagnitude);
};
