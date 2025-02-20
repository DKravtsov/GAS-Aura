// Copyright 4sandwiches


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "Logs.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

    if (!DelegateHandle_GameplayEffectAppliedToSelf.IsValid())
    {
        DelegateHandle_GameplayEffectAppliedToSelf = 
            OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
    }
}

void UAuraAbilitySystemComponent::GrantStartupAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& StartupAbilities, int32 AbilityLevel)
{
    for (const auto& AbilityClass : StartupAbilities)
    {
        auto Spec = FGameplayAbilitySpec(AbilityClass, AbilityLevel);
        Spec.SourceObject = GetAvatarActor();
        if (const auto AuraAbility = Cast<UAuraGameplayAbility>(Spec.Ability))
        {
            if (AuraAbility->StartupInputTag.IsValid())
            {
                Spec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
            }
        }
        GiveAbility(Spec);
    }
    bStartupAbilitiesGiven = true;
    OnAbilitiesGiven.Broadcast();
}

void UAuraAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
        return;

    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputPressed(Spec);
            const bool bActiveAbility = Spec.IsActive();
            if (!bActiveAbility)
            {
                TryActivateAbility(Spec.Handle);
            }
        }
    }
}

void UAuraAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
        return;

    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputReleased(Spec);
        }
    }
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer AssetTags;
    EffectSpec.GetAllAssetTags(AssetTags);
    OnEffectApplied.Broadcast(AssetTags);
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbilityDelegate& Delegate)
{
    if (!ensureAlways(Delegate.IsBound()))
    {
        LOG_ERROR(TEXT("Failed to execute delegate."));
        return;
    }
    FScopedAbilityListLock ActiveScopeLock(*this);
    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        Delegate.Execute(AbilitySpec);
    }
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (const FGameplayTag& Tag : AbilitySpec.Ability->AbilityTags)
    {
        if (Tag.MatchesTag(AuraGameplayTags::Abilities))
        {
            return Tag;
        }
    }
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
    {
        if (Tag.MatchesTag(AuraGameplayTags::InputTag))
        {
            return Tag;
        }
    }
    return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
    Super::OnRep_ActivateAbilities();

    if (!bStartupAbilitiesGiven)
    {
        bStartupAbilitiesGiven = true;
        OnAbilitiesGiven.Broadcast();
    }
}

