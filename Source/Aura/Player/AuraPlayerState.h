// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedDelegate, const int32 /*Value*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerLevelChangedDelegate, const int32 /*Value*/, bool /*bLevelUp*/);

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AuraPlayerState")
    TObjectPtr<class ULevelUpDataAsset> LevelUpData;

    FOnPlayerStatChangedDelegate OnXPChanged;
    FOnPlayerLevelChangedDelegate OnLevelChanged;
    FOnPlayerStatChangedDelegate OnAttributePointsChanged;
    FOnPlayerStatChangedDelegate OnSpellPointsChanged;
    
protected:

    UPROPERTY()
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<class UAttributeSet> AttributeSet;

private:

    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
    int32 Level = 1;

    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentXP)
    int32 CurrentXP = 0;

    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
    int32 AttributePoints = 0;

    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
    int32 SpellPoints = 0;

public:

    AAuraPlayerState();

    //~ Begin of IAbilitySystemInterface interface
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End of IAbilitySystemInterface interface

    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

    class UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;
    class UAuraAbilitySystemComponent* GetAuraAbilitySystemComponentChecked() const;
    class UAuraAttributeSet* GetAuraAttributeSetChecked() const;

    FORCEINLINE int32 GetPlayerLevel() const { return Level; }
    void SetPlayerLevel(int32 NewLevel);
    void AddPlayerLevel(int32 NumLevels = 1);
    void InitPlayerLevel(int32 NewLevel);

    FORCEINLINE int32 GetXP() const { return CurrentXP; }
    void SetXP(int32 NewValue);
    void AddXP(int32 Value);
    void InitXP(int32 NewValue);

    FORCEINLINE int32 GetAttributePoints() const {return AttributePoints; }
    void SetAttributePoints(int32 NewValue);
    void AddAttributePoints(int32 Amount);
    void InitAttributePoints(int32 NewValue);

    FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
    void SetSpellPoints(int32 NewValue);
    void AddSpellPoints(int32 Amount);
    void InitSpellPoints(int32 NewValue);

    void UpgradeAttribute(const struct FGameplayTag& AttributeTag, const int32 Points = 1);

    UFUNCTION(Server, WithValidation, Reliable)
    void ServerUpgradeAttribute(const FGameplayTag& AttributeTag, const int32 Points = 1);

    void UpgradeSpell(const FGameplayTag& AbilityTag);

    UFUNCTION(Server, WithValidation, Reliable)
    void ServerUpgradeSpell(const FGameplayTag& AbilityTag);
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

    UFUNCTION()
    void OnRep_Level(int32 OldValue);

    UFUNCTION()
    void OnRep_CurrentXP(int32 OldValue);

    UFUNCTION()
    void OnRep_AttributePoints(int32 OldValue);

    UFUNCTION()
    void OnRep_SpellPoints(int32 OldValue);
};
