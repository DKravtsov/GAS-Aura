// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TargetDataUnderCursor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCursorTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 *
 */
UCLASS()
class AURA_API UAbilityTask_TargetDataUnderCursor : public UAbilityTask
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable)
    FCursorTargetDataSignature CursorTargetData;

private:

    ECollisionChannel TraceChannel = ECC_Visibility;
    bool bTraceComplex = false;

public:

    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderCursor", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true, AdvancedDisplay = "1"))
    static UAbilityTask_TargetDataUnderCursor* CreateTargetDataUnderCursorProxy(UGameplayAbility* OwningAbility,
        ECollisionChannel TraceChannel = ECC_Visibility, bool bTraceComplex = false);

protected:

    virtual void Activate() override;

private:

    void SendCursorTargetData();

    void OnReceivedTargetData(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivaztionTag);
};
