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

public:

    AAuraPlayerState();

    //~ Begin of IAbilitySystemInterface interface
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End of IAbilitySystemInterface interface

    UAttributeSet* GetAttributeSet() const { return AttributeSet; }
};
