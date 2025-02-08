// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

protected:

    UPROPERTY()
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<class UAttributeSet> AttributeSet;

private:

    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
    int32 Level = 1;

public:

    AAuraPlayerState();

    //~ Begin of IAbilitySystemInterface interface
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End of IAbilitySystemInterface interface

    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

    FORCEINLINE int32 GetPlayerLevel() const { return Level; }

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

    UFUNCTION()
    void OnRep_Level(int32 OldValue);
};
