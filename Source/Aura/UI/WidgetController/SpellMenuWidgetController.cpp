// Copyright 4sandwiches


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfoDataAsset.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
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
}
