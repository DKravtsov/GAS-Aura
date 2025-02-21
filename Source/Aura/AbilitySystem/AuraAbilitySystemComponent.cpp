// Copyright 4sandwiches


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "Logs.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Data/AbilityInfoDataAsset.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

    if (!DelegateHandle_GameplayEffectAppliedToSelf.IsValid())
    {
        DelegateHandle_GameplayEffectAppliedToSelf = 
            OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
    }
}

void UAuraAbilitySystemComponent::GrantAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& Abilities, int32 AbilityLevel)
{
    for (const auto& AbilityClass : Abilities)
    {
        auto Spec = FGameplayAbilitySpec(AbilityClass, AbilityLevel);
        //Spec.SourceObject = GetAvatarActor();
        if (const auto AuraAbility = Cast<UAuraGameplayAbility>(Spec.Ability))
        {
            Spec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
        }
        Spec.DynamicAbilityTags.AddTag(AuraGameplayTags::Abilities_Status_Equipped);
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

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
    {
        if (Tag.MatchesTag(AuraGameplayTags::Abilities_Status))
        {
            return Tag;
        }
    }
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
    {
        if (Tag.MatchesTag(AuraGameplayTags::Abilities_Type))
        {
            return Tag;
        }
    }
    return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::FindAbilitySpecByAbilityTag(const FGameplayTag& AbilityTag)
{
    FScopedAbilityListLock ActiveScopeLock(*this);
    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        for (FGameplayTag Tag : Spec.Ability->AbilityTags)
        {
            if (Tag.MatchesTag(AbilityTag))
                return &Spec;
        }
    }
    return nullptr;
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
    auto AbilityInfo = UAuraBlueprintFunctionLibrary::GetAbilityInfo(GetAvatarActor());

    for (const auto& Info : AbilityInfo->AbilityInfos)
    {
        if (Level < Info.LevelRequirement || !Info.AbilityTag.IsValid())
            continue;;
        
        // TArray<FGameplayAbilitySpecHandle> Handles;
        // FindAllAbilitiesWithTags(Handles, Info.AbilityTag.GetSingleTagContainer());

        if (FindAbilitySpecByAbilityTag(Info.AbilityTag) == nullptr)
        {
            constexpr int32 AbilityLevel = 1;
            auto Spec = FGameplayAbilitySpec(Info.AbilityClass, AbilityLevel);
            // Changed status Locked -> Eligible
            Spec.DynamicAbilityTags.AddTag(AuraGameplayTags::Abilities_Status_Eligible);
            
            GiveAbility(Spec);
            MarkAbilitySpecDirty(Spec);

            //OnAbilityStatusChange.Broadcast(Info.AbilityTag, AuraGameplayTags::Abilities_Status_Eligible);
            ClientUpdateAbilityStatus(Info.AbilityTag, AuraGameplayTags::Abilities_Status_Eligible);
        }
    }
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

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
{
    LOG_NETFUNCTIONCALL_COMPONENT;
    OnAbilityStatusChange.Broadcast(AbilityTag, StatusTag);
}

