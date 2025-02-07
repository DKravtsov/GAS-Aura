// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, Category = Combat)
    TObjectPtr<class USkeletalMeshComponent> Weapon;

    UPROPERTY()
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<class UAttributeSet> AttributeSet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    TSubclassOf<class UGameplayEffect> DefaultPrimaryAttributes;

public:

    AAuraCharacterBase();

    //~ Begin of IAbilitySystemInterface interface
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End of IAbilitySystemInterface interface

    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:

    virtual void BeginPlay() override;

    void InitializePrimaryAttributes();

};
