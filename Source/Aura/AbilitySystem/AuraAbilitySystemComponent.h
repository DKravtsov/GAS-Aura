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

protected:

    /** Called on server whenever a GE is applied to self. This includes instant and duration based GEs. */
    void EffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

private:
    FDelegateHandle DelegateHandle_GameplayEffectAppliedToSelf;
};
