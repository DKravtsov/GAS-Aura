// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectAppliedSignature, const FGameplayTagContainer& /*AssetTags*/);

/**
 *
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:

    FOnEffectAppliedSignature OnEffectApplied;

public:

    void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

    void GrantStartupAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& StartupAbilities, int32 AbilityLevel);

    void AbilityInputPressed(const FGameplayTag& InputTag);
    void AbilityInputReleased(const FGameplayTag& InputTag);

protected:

    UFUNCTION(Client, Reliable)
    void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

private:
    FDelegateHandle DelegateHandle_GameplayEffectAppliedToSelf;
};
