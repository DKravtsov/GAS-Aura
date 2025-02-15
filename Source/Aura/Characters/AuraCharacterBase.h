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
    
    UPROPERTY()
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<class UAttributeSet> AttributeSet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class USkeletalMeshComponent> Weapon;

    UPROPERTY(EditAnywhere, Category = Combat, BlueprintReadOnly)
    FName WeaponTipSocketName;

    UPROPERTY(EditAnywhere, Category = Combat, BlueprintReadOnly)
    FName LeftHandSocketName;

    UPROPERTY(EditAnywhere, Category = Combat, BlueprintReadOnly)
    FName RightHandSocketName;

    UPROPERTY(EditAnywhere, Category = "Attributes")
    TSubclassOf<class UGameplayEffect> DefaultPrimaryAttributes;

    UPROPERTY(EditAnywhere, Category = "Attributes")
    TSubclassOf<class UGameplayEffect> DefaultSecondaryAttributes;

    UPROPERTY(EditAnywhere, Category = "Attributes")
    TSubclassOf<class UGameplayEffect> DefaultVitalAttributes;

    UPROPERTY(EditAnywhere, Category = "Attributes")
    TArray<TSubclassOf<class UGameplayEffect>> StartupGameplayEffects;

    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
    TObjectPtr<class UAnimMontage> HitReactMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
    class UNiagaraSystem* BloodEffect;

    UPROPERTY(BlueprintReadOnly, Category = Combat)
    bool bDead = false;


public:

    AAuraCharacterBase();

    //~ Begin of IAbilitySystemInterface interface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End of IAbilitySystemInterface interface

    class UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;

    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

    UFUNCTION(BlueprintCallable)
    void ApplyGameplayEffectToSelf(TSubclassOf<class UGameplayEffect> Effect, float EffectLevel = 1.f);

    //~ Begin of ICombatInterface interface
    virtual FVector GetCombatSocketLocation_Implementation(FGameplayTag CombatSocketTag) const override;
    virtual UAnimMontage* GetHitReactAnimMontage_Implementation() const override { return HitReactMontage; }
    virtual void Die() override;
    virtual bool IsDead_Implementation() const override;
    virtual AActor* GetAvatar_Implementation() override;
    virtual UNiagaraSystem* GetBloodEffect_Implementation() const override { return BloodEffect; }
    //~ End of ICombatInterface interface

protected:

    virtual void BeginPlay() override;

    virtual void InitializeDefaultAttributes();

    virtual void GrantStartupAbilities();

    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastHandleDeath();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void DissolveDeadBody();
    
};
