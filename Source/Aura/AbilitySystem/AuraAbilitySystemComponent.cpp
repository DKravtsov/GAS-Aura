// Copyright 4sandwiches


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "Logs.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Data/AbilityInfoDataAsset.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "AuraGameplayAbility"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

    if (!DelegateHandle_GameplayEffectAppliedToSelf.IsValid())
    {
        DelegateHandle_GameplayEffectAppliedToSelf = 
            OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
    }
}

void UAuraAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
    Super::OnGiveAbility(AbilitySpec);

    if (bStartupAbilitiesGiven)
    {
        // broadcast new ability info
        OnAbilitiesGiven.Broadcast();
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
            if (Spec.IsActive())
            {
                InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
            }
        }
    }
}

void UAuraAbilitySystemComponent::AbilityInputHeld(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
        return;

    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputPressed(Spec);
            if (!Spec.IsActive())
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
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag) && Spec.IsActive())
        {
            AbilitySpecInputReleased(Spec);
            InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
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

FGameplayTag UAuraAbilitySystemComponent::GetAbilityInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
    if (const auto Spec = FindAbilitySpecByAbilityTag(AbilityTag))
    {
        return GetInputTagFromSpec(*Spec);
    }
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
    if (const auto Spec = FindAbilitySpecByAbilityTag(AbilityTag))
    {
        return GetStatusTagFromSpec(*Spec);
    }
    return FGameplayTag();
}

bool UAuraAbilitySystemComponent::GetAbilityDescriptionsByTag(const FGameplayTag& AbilityTag, const int32 Level, FText& OutDesc, FText& OutNextLevelDesc)
{
    if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecByAbilityTag(AbilityTag))
    {
        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
        {
            AuraAbility->GetDescription(Level, OutDesc, OutNextLevelDesc);
            return true;
        }
    }
    
    if (const UAbilityInfoDataAsset* AbilityInfo = UAuraBlueprintFunctionLibrary::GetAbilityInfo(GetAvatarActor()))
    {
        const auto Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
        static const FTextFormat LockedTextFormat = LOCTEXT("LockedSpellDesc", "Required level <Level>{0}</> to unlock."); 
        OutDesc = FText::Format(LockedTextFormat, Info.LevelRequirement);
        OutNextLevelDesc = FText::GetEmpty();
    }
    return false;
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 CharacterLevel)
{
    if (const auto AbilityInfo = UAuraBlueprintFunctionLibrary::GetAbilityInfo(GetAvatarActor()))
    {
        for (const auto& Info : AbilityInfo->AbilityInfos)
        {
            if (CharacterLevel < Info.LevelRequirement || !Info.AbilityTag.IsValid())
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

                ClientUpdateAbilityStatus(Info.AbilityTag, AuraGameplayTags::Abilities_Status_Eligible, Spec.Level);
            }
        }
    }
}

bool UAuraAbilitySystemComponent::UpgradeAbility(const FGameplayTag& AbilityTag)
{
    auto AbilitySpec = FindAbilitySpecByAbilityTag(AbilityTag);
    if (AbilitySpec == nullptr)
    {
        UE_LOG(LogMyGame, Warning, TEXT("Failed to upgrade ability [%s]: Not found."), *AbilityTag.ToString());
        return false;
    }

       
    FGameplayTag Status = GetStatusTagFromSpec(*AbilitySpec);
    if (Status.MatchesTagExact(AuraGameplayTags::Abilities_Status_Eligible))
    {
        // Unlock the ability
        AbilitySpec->DynamicAbilityTags.RemoveTag(AuraGameplayTags::Abilities_Status_Eligible);
        AbilitySpec->DynamicAbilityTags.AddTag(AuraGameplayTags::Abilities_Status_Unlocked);
        Status = AuraGameplayTags::Abilities_Status_Unlocked;
    }
    else if (IsStatusEquipOrUnlocked(Status))
    {
        // level up the ability
        AbilitySpec->Level++;
    }
    else
    {
        UE_LOG(LogMyGame, Warning, TEXT("Failed to upgrade ability [%s]: Invalid status [%s]."),
            *AbilityTag.ToString(), *Status.ToString());
        return false;
    }

    ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
    MarkAbilitySpecDirty(*AbilitySpec);
    
    return true;
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

bool UAuraAbilitySystemComponent::IsStatusEquipOrUnlocked(const FGameplayTag& InStatusTag)
{
    static auto CreateTagContainer = []()
    {
        TArray<FGameplayTag> Array = {AuraGameplayTags::Abilities_Status_Equipped, AuraGameplayTags::Abilities_Status_Unlocked};
        return FGameplayTagContainer::CreateFromArray(Array);
    };
        
    static const FGameplayTagContainer EquipOrUnlocked = CreateTagContainer();

    return InStatusTag.MatchesAnyExact(EquipOrUnlocked);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 Level)
{
    LOG_NETFUNCTIONCALL_COMPONENT;
    OnAbilityStatusChange.Broadcast(AbilityTag, StatusTag, Level);
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& InAbilityTag,  const FGameplayTag& InInputTag)
{
    if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecByAbilityTag(InAbilityTag))
    {
        const FGameplayTag PrevSlot = GetInputTagFromSpec(*AbilitySpec);
        const FGameplayTag Status = GetStatusTagFromSpec(*AbilitySpec);
        const bool bValidStatus = IsStatusEquipOrUnlocked(Status);

        if (!bValidStatus)
            return;

        ClearAbilitiesOfInputSlot(InInputTag);

        ClearInputSlot(*AbilitySpec);

        AbilitySpec->DynamicAbilityTags.AddTag(InInputTag);
        
        AbilitySpec->DynamicAbilityTags.RemoveTag(AuraGameplayTags::Abilities_Status_Unlocked);//won't do anything if there's no such tag
        AbilitySpec->DynamicAbilityTags.AddTag(AuraGameplayTags::Abilities_Status_Equipped);//won't add tag if already exists

        MarkAbilitySpecDirty(*AbilitySpec);

        ClientEquipAbility(InAbilityTag, InInputTag, Status, PrevSlot);
    }
}

bool UAuraAbilitySystemComponent::ServerEquipAbility_Validate(const FGameplayTag& InAbilityTag, const FGameplayTag& InInputTag)
{
    return true;
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
    const FGameplayTag& InputTag, const FGameplayTag& Status, const FGameplayTag& PrevSlot)
{
    OnAbilityEquipped.Broadcast(AbilityTag, Status, InputTag, PrevSlot);
}

void UAuraAbilitySystemComponent::ClearInputSlot(FGameplayAbilitySpec& Spec)
{
    const FGameplayTag Slot = GetInputTagFromSpec(Spec);
    Spec.DynamicAbilityTags.RemoveTag(Slot);
    MarkAbilitySpecDirty(Spec);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfInputSlot(const FGameplayTag& InSlot)
{
    FScopedAbilityListLock Lock(*this);
    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        if (HasAbilitySlot(Spec, InSlot))
        {
            ClearInputSlot(Spec);
        }
    }
}

bool UAuraAbilitySystemComponent::HasAbilitySlot(FGameplayAbilitySpec& Spec, const FGameplayTag& InSlot)
{
    for (const FGameplayTag& Tag : Spec.DynamicAbilityTags)
    {
        if (Tag.MatchesTagExact(InSlot))
        {
            return true;
        }
    }
   return false;
}

#undef LOCTEXT_NAMESPACE
