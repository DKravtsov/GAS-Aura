// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectAppliedSignature, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGivenSignature);
DECLARE_DELEGATE_OneParam(FForEachAbilityDelegate, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChangeDelegate, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, const int32 /*Level*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquippedDelegate, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*Status*/, const FGameplayTag& /*Slot*/, const FGameplayTag& /*PrevSlot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FPassiveAbilityDeactivatedDelegate, const FGameplayTag& /*AbilityTag*/);


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
    FAbilityEquippedDelegate OnAbilityEquipped;
    FPassiveAbilityDeactivatedDelegate OnPassiveAbilityDeactivated;
    
public:

    //~ Begin of UAbilitySystemComponent interface
    virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
    virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
    //~ End of UAbilitySystemComponent interface

    void GrantAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& Abilities, int32 AbilityLevel);

    void AbilityInputPressed(const FGameplayTag& InputTag);
    void AbilityInputHeld(const FGameplayTag& InputTag);
    void AbilityInputReleased(const FGameplayTag& InputTag);

    bool AreStartupAbilitiesGiven() const { return bStartupAbilitiesGiven; }

    void ForEachAbility(const FForEachAbilityDelegate& Delegate);

    static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

    UFUNCTION(BlueprintCallable)
    FGameplayTag GetAbilityInputTagFromAbilityTag(const FGameplayTag& AbilityTag);

    UFUNCTION(BlueprintCallable)
    FGameplayTag GetAbilityStatusFromAbilityTag(const FGameplayTag& AbilityTag);

    FGameplayAbilitySpec* FindAbilitySpecByAbilityTag(const FGameplayTag& AbilityTag);

    bool GetAbilityDescriptionsByTag(const FGameplayTag& AbilityTag, int32 Level, FText& OutDesc, FText& OutNextLevelDesc);
    
    void UpdateAbilityStatuses(int32 Level);

    bool UpgradeAbility(const FGameplayTag& AbilityTag);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerEquipAbility(const FGameplayTag& InAbilityTag, const FGameplayTag& InInputTag);

protected:

    UFUNCTION(Client, Reliable)
    void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

    //~ Begin of UAbilitySystemComponent interface
    virtual void OnRep_ActivateAbilities() override;
    //~ End of UAbilitySystemComponent interface

    UFUNCTION(Client, Reliable)
    void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 Level);

    UFUNCTION(Client, Reliable)
    void ClientEquipAbility(const FGameplayTag& InAbilityTag, const FGameplayTag& InInputTag, const FGameplayTag& InStatus, const FGameplayTag& InPrevSlot);

    static bool IsStatusEquipOrUnlocked(const FGameplayTag& InStatusTag);

    void ClearInputSlot(FGameplayAbilitySpec& Spec);
    void ClearAbilitiesOfInputSlot(const FGameplayTag& InSlot);
    static bool HasAbilitySlot(FGameplayAbilitySpec& Spec, const FGameplayTag& InSlot);
    
private:
    FDelegateHandle DelegateHandle_GameplayEffectAppliedToSelf;
    bool bStartupAbilitiesGiven = false;

};
