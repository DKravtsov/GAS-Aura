// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectAppliedSignature, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGivenSignature);
DECLARE_DELEGATE_OneParam(FForEachAbilityDelegate, const FGameplayAbilitySpec&);
/**
 *
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:

    FOnEffectAppliedSignature OnEffectApplied;
    FAbilitiesGivenSignature OnAbilitiesGiven;
    
public:

    //~ Begin of UAbilitySystemComponent interface
    virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
    //~ End of UAbilitySystemComponent interface

    void GrantStartupAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& StartupAbilities, int32 AbilityLevel);

    void AbilityInputPressed(const FGameplayTag& InputTag);
    void AbilityInputReleased(const FGameplayTag& InputTag);

    bool AreStartupAbilitiesGiven() const { return bStartupAbilitiesGiven; }

    void ForEachAbility(const FForEachAbilityDelegate& Delegate);

    static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

protected:

    UFUNCTION(Client, Reliable)
    void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

    //~ Begin of UAbilitySystemComponent interface
    virtual void OnRep_ActivateAbilities() override;
    //~ End of UAbilitySystemComponent interface

private:
    FDelegateHandle DelegateHandle_GameplayEffectAppliedToSelf;
    bool bStartupAbilitiesGiven = false;

};
