// Copyright 4sandwiches


#include "AbilitySystem/AsyncTask/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Algo/MaxElement.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(class UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayTag InCooldownTag)
{
	UWaitCooldownChange* Proxy = NewObject<UWaitCooldownChange>();
	Proxy->AbilitySystemComponent = InAbilitySystemComponent;
	Proxy->CooldownTag = InCooldownTag;

	if (!IsValid(InAbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		Proxy->EndTask();
		return nullptr;
	}

	return Proxy;
}

void UWaitCooldownChange::Activate()
{
	Super::Activate();

	AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UWaitCooldownChange::CooldownTagChanged);
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UWaitCooldownChange::OnActiveEffectAdded);
}

void UWaitCooldownChange::EndTask()
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* ASC,	const struct FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	EffectSpec.GetAllAssetTags(AssetTags);
	FGameplayTagContainer GrantedTags;
	EffectSpec.GetAllGrantedTags(GrantedTags);

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		auto Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimeRemainingArray = AbilitySystemComponent->GetActiveEffectsTimeRemaining(Query);
		if (!TimeRemainingArray.IsEmpty())
		{
			float TimeRemaining = *Algo::MaxElement(TimeRemainingArray);
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
