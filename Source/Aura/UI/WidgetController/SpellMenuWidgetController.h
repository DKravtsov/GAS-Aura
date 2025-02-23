// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
    
	UPROPERTY(BlueprintAssignable, Category="GAS|Player Stats")
	FOnIntStatChangedSignature OnSpellPointsChanged;

public:

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacks() override;

	UFUNCTION(BlueprintCallable)
	void UpgradeSpell(const struct FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	bool GetSpellDescription(const FGameplayTag& AbilityTag, FText& OutDescription, FText& OutNextLevelDescription);

	UFUNCTION(BlueprintCallable)
	void EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	void UnEquipAbility(const FGameplayTag& InputTag);
};
