// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class AURA_API ICombatInterface
{
    GENERATED_BODY()

public:

    virtual int32 GetCharacterLevel() const { return 0; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FVector GetCombatSocketLocation() const;

    // Set Target Location for anim montages with MotionWarping or similar
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateFacingTarget(const FVector& TargetLocation);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    class UAnimMontage* GetHitReactAnimMontage() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetCombatTarget(AActor* TargetActor);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    AActor* GetCombatTarget() const;
    
    virtual void Die() = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsDead() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    AActor* GetAvatar();
};
