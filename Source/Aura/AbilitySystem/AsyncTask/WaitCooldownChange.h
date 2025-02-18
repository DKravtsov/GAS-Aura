// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

private:

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	FGameplayTag CooldownTag;
	
public:
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	static UWaitCooldownChange* WaitForCooldownChange(class UAbilitySystemComponent* InAbilitySystemComponent,
		UPARAM(meta=(Categories="Cooldown")) const FGameplayTag InCooldownTag);

	virtual void Activate() override;
	
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:

	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	void OnActiveEffectAdded(UAbilitySystemComponent* ASC, const struct FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};
