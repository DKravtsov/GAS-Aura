// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "Characters/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
    GENERATED_BODY()
public:

    UPROPERTY(BlueprintAssignable)
    FOnDeathSignature OnDeath;
    
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

    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> StartupPassiveAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
    TObjectPtr<class UAnimMontage> HitReactMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
    class UNiagaraSystem* BloodEffect;

    UPROPERTY(BlueprintReadOnly, Category = Combat)
    bool bDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
    TObjectPtr<USoundBase> DeathSound;

    int32 MinionCount = 0;

    // default value (-1) will be overwritten by class defaults
    UPROPERTY(EditDefaultsOnly, Category=Combat)
    FScalableFloat RewardXP;
    
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
    virtual int32 GetMinionCount_Implementation() const override { return MinionCount; }
    virtual APawn* SummonMinion_Implementation(TSubclassOf<APawn> MinionClass, FVector Location, FRotator Rotation) override;
    virtual FOnDeathSignature& GetOnDeathDelegate() override { return OnDeath; };
    //~ End of ICombatInterface interface

    int32 GetCharacterLevel() const;
    
protected:

    virtual void BeginPlay() override;

    virtual void InitializeDefaultAttributes();

    virtual void GrantStartupAbilities();

    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastHandleDeath();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void DissolveDeadBody();

    virtual void AddedMinion(APawn* NewMinion) {}
    virtual void RemovedMinion(APawn* Minion) {}

    // plays on both server and client
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "HandleDeath"))
    void BP_HandleDeath();
};
