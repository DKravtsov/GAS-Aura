// Copyright 4sandwiches


#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderCursor.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"

UAbilityTask_TargetDataUnderCursor* UAbilityTask_TargetDataUnderCursor::CreateTargetDataUnderCursorProxy(UGameplayAbility* OwningAbility,
    ECollisionChannel TraceChannel /*= ECC_Visibility*/, bool bTraceComplex /*= false*/)
{
    UAbilityTask_TargetDataUnderCursor* Proxy = NewAbilityTask<UAbilityTask_TargetDataUnderCursor>(OwningAbility);
    Proxy->TraceChannel = TraceChannel;
    Proxy->bTraceComplex = bTraceComplex;
    return Proxy;
}

void UAbilityTask_TargetDataUnderCursor::Activate()
{
    Super::Activate();

    if (IsLocallyControlled())
    {
        SendCursorTargetData();
    }
    else
    {
        // we're on server

        AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnReceivedTargetData);
        if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(), GetActivationPredictionKey()))
        {
            SetWaitingOnRemotePlayerData();
        }
    }
}

void UAbilityTask_TargetDataUnderCursor::SendCursorTargetData()
{
    if (APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
    {
        FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

        FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
        PC->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(TraceChannel), bTraceComplex, Data->HitResult);

        FGameplayAbilityTargetDataHandle DataHandle(Data);

        AbilitySystemComponent->ServerSetReplicatedTargetData(
            GetAbilitySpecHandle(), 
            GetActivationPredictionKey(), 
            DataHandle, 
            FGameplayTag(),
            AbilitySystemComponent->ScopedPredictionKey);

        if (ShouldBroadcastAbilityTaskDelegates())
        {
            CursorTargetData.Broadcast(DataHandle);
        }
    }
}

void UAbilityTask_TargetDataUnderCursor::OnReceivedTargetData(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivaztionTag)
{
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        CursorTargetData.Broadcast(DataHandle);
    }
}
