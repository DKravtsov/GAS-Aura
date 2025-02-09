// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Characters/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, Category = Combat, BlueprintReadOnly)
    TObjectPtr<class USkeletalMeshComponent> Weapon;

    UPROPERTY(EditAnywhere, Category = Combat, BlueprintReadOnly)
    FName WeaponTipSocketName;

    UPROPERTY()
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<class UAttributeSet> AttributeSet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    TSubclassOf<class UGameplayEffect> DefaultPrimaryAttributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    TSubclassOf<class UGameplayEffect> DefaultSecondaryAttributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    TSubclassOf<class UGameplayEffect> DefaultVitalAttributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    TArray<TSubclassOf<class UGameplayEffect>> StartupGameplayEffects;

private:

    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

public:

    AAuraCharacterBase();

    //~ Begin of IAbilitySystemInterface interface
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End of IAbilitySystemInterface interface

    class UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;

    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

    UFUNCTION(BlueprintCallable)
    void ApplyGameplayEffectToSelf(TSubclassOf<class UGameplayEffect> Effect, float EffectLevel = 1.f);

    //~ Begin of ICombatInterface interface
    FVector GetCombatSocketLocation() const override;
    //~ End of ICombatInterface interface

protected:

    virtual void BeginPlay() override;

    void InitializeDefaultAttributes();

    void GrantStartupAbilities();

};
