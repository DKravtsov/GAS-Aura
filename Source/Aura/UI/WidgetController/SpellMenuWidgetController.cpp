// Copyright 4sandwiches


#include "UI/WidgetController/SpellMenuWidgetController.h"

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