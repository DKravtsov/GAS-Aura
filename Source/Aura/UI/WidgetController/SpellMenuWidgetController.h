// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
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
};
