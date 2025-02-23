// Copyright 4sandwiches


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "Logs.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfoDataAsset.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
    OnSpellPointsChanged.Broadcast(AuraPlayerState->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacks()
{
	AuraAbilitySystemComponent->OnAbilityStatusChange
		.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 Level)
		{
			if (AbilityInfo)
			{
				auto Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
			    Info.StatusTag = StatusTag;
				Info.SpellLevel = Level;
	            AuraAbilitySystemComponent->GetAbilityDescriptionsByTag(AbilityTag, Info.Description, Info.NextLevelDescription);
			    OnReceivedAbilityInfo.Broadcast(Info);
			}
		});
	AuraPlayerState->OnSpellPointsChanged.AddLambda([this](const int32 SP)
		{
			OnSpellPointsChanged.Broadcast(SP); 
		});
}

void USpellMenuWidgetController::UpgradeSpell(const FGameplayTag& AbilityTag)
{
	if (AuraPlayerState)
	{
		AuraPlayerState->UpgradeSpell(AbilityTag);
	}
}

bool USpellMenuWidgetController::GetSpellDescription(const FGameplayTag& AbilityTag, FText& OutDescription, FText& OutNextLevelDescription)
{
	if (!IsValid(AuraAbilitySystemComponent))
		return false;
	return AuraAbilitySystemComponent->GetAbilityDescriptionsByTag(AbilityTag, OutDescription, OutNextLevelDescription);
}

void USpellMenuWidgetController::EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	UE_LOG(LogMyGame, Warning, TEXT("SpellMenuWidgetController (%s): Equip Ability [%s] to slot [%s]"),
		 NETMODE_WORLD, *AbilityTag.ToString(), *InputTag.ToString());
}

void USpellMenuWidgetController::UnEquipAbility(const FGameplayTag& InputTag)
{
	UE_LOG(LogMyGame, Warning, TEXT("SpellMenuWidgetController (%s): UnEquip Ability from slot [%s]"),
		NETMODE_WORLD, *InputTag.ToString());
}
