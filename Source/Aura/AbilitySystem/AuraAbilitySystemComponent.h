// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectAppliedSignature, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGivenSignature);
DECLARE_DELEGATE_OneParam(FForEachAbilityDelegate, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChangeDelegate, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, const int32 /*Level*/);

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
    FAbilityStatusChangeDelegate OnAbilityStatusChange;
    
public:

    //~ Begin of UAbilitySystemComponent interface
    virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
    //~ End of UAbilitySystemComponent interface

    void GrantAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& Abilities, int32 AbilityLevel);

    void AbilityInputPressed(const FGameplayTag& InputTag);
    void AbilityInputReleased(const FGameplayTag& InputTag);

    bool AreStartupAbilitiesGiven() const { return bStartupAbilitiesGiven; }

    void ForEachAbility(const FForEachAbilityDelegate& Delegate);

    static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

    FGameplayAbilitySpec* FindAbilitySpecByAbilityTag(const FGameplayTag& AbilityTag);

    bool GetAbilityDescriptionsByTag(const FGameplayTag& AbilityTag, FText& OutDesc, FText& OutNextLevelDesc);
    
    void UpdateAbilityStatuses(int32 Level);

    bool UpgradeAbility(const FGameplayTag& AbilityTag);
    
protected:

    UFUNCTION(Client, Reliable)
    void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

    //~ Begin of UAbilitySystemComponent interface
    virtual void OnRep_ActivateAbilities() override;
    //~ End of UAbilitySystemComponent interface

    UFUNCTION(Client, Reliable)
    void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 Level);
    
private:
    FDelegateHandle DelegateHandle_GameplayEffectAppliedToSelf;
    bool bStartupAbilitiesGiven = false;

};
