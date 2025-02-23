// Copyright 4sandwiches


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "Logs.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfoDataAsset.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
    OnSpellPointsChanged.Broadcast(AuraPlayerState->GetSpellPoints());
	BroadcastAllAbilitiesInitInfo();
}

void USpellMenuWidgetController::BindCallbacks()
{
	AuraAbilitySystemComponent->OnAbilityStatusChange
		.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 Level)
		{
			OnAbilityStatusChanged.Broadcast(AbilityTag, StatusTag, Level);
		});
	AuraPlayerState->OnSpellPointsChanged.AddLambda([this](const int32 SP)
		{
			OnSpellPointsChanged.Broadcast(SP); 
		});
	AuraAbilitySystemComponent->OnAbilitiesGiven.AddUObject(this, &USpellMenuWidgetController::BroadcastAbilityInfo);
	AuraAbilitySystemComponent->OnAbilityEquipped.AddUObject(this, &USpellMenuWidgetController::AbilityEquipped);
}

void USpellMenuWidgetController::UpgradeSpell(const FGameplayTag& AbilityTag)
{
	if (AuraPlayerState)
	{
		AuraPlayerState->UpgradeSpell(AbilityTag);
	}
}

bool USpellMenuWidgetController::GetSpellDescription(const FGameplayTag& AbilityTag, const int32 Level, FText& OutDescription, FText& OutNextLevelDescription)
{
	if (!IsValid(AuraAbilitySystemComponent))
		return false;
	return AuraAbilitySystemComponent->GetAbilityDescriptionsByTag(AbilityTag, Level, OutDescription, OutNextLevelDescription);
}

void USpellMenuWidgetController::EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	UE_LOG(LogMyGame, Warning, TEXT("SpellMenuWidgetController (%s): Equip Ability [%s] to slot [%s]"),
		 NETMODE_WORLD, *AbilityTag.ToString(), *InputTag.ToString());

	if (IsValid(AuraAbilitySystemComponent))
	{
		AuraAbilitySystemComponent->ServerEquipAbility(AbilityTag, InputTag);
	}
}

void USpellMenuWidgetController::BroadcastAllAbilitiesInitInfo()
{
	if (AbilityInfo && IsValid(AuraAbilitySystemComponent))
	{
		for (const auto& InfoRef : AbilityInfo->AbilityInfos)
		{
			auto Info = FAuraAbilityInfo(InfoRef, true);
			if (const FGameplayAbilitySpec* AbilitySpec = AuraAbilitySystemComponent->FindAbilitySpecByAbilityTag(Info.AbilityTag))
			{
				Info.SetInputTag(UAuraAbilitySystemComponent::GetInputTagFromSpec(*AbilitySpec));
				Info.SetStatus(UAuraAbilitySystemComponent::GetStatusTagFromSpec(*AbilitySpec));
				Info.SetSpellLevel(AbilitySpec->Level);
			}
			OnReceivedAbilityInfo.Broadcast(Info);
		}
	}
}
