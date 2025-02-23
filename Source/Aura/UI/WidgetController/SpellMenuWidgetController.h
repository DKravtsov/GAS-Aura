// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChangedSignature, const FGameplayTag&, AbilityTag, const FGameplayTag&, StatusTag, const int32, Level);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
    
	UPROPERTY(BlueprintAssignable, Category="GAS|Player Stats")
	FOnIntStatChangedSignature OnSpellPointsChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Player Stats")
	FAbilityStatusChangedSignature OnAbilityStatusChanged;
	
public:

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacks() override;

	UFUNCTION(BlueprintCallable)
	void UpgradeSpell(const struct FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	bool GetSpellDescription(const FGameplayTag& AbilityTag, const int32 Level, FText& OutDescription, FText& OutNextLevelDescription);

	UFUNCTION(BlueprintCallable)
	void EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

protected:

	void BroadcastAllAbilitiesInitInfo();

};
