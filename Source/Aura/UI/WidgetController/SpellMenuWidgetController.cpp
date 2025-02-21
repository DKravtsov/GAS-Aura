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
		.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (AbilityInfo)
			{
				auto Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
			   Info.StatusTag = StatusTag;
			   OnReceivedAbilityInfo.Broadcast(Info);
			}
		});
	AuraPlayerState->OnSpellPointsChanged.AddLambda([this](const int32 SP)
		{
			OnSpellPointsChanged.Broadcast(SP); 
		});
}

void USpellMenuWidgetController::UpgradeSpell(const FGameplayTag& AttributeTag)
{
}
