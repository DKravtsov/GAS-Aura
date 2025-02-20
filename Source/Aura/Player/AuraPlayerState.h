// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedDelegate, const int32 /*Value*/);

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
    FOnPlayerStatChangedDelegate OnLevelChanged;
    
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

public:

    AAuraPlayerState();

    //~ Begin of IAbilitySystemInterface interface
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End of IAbilitySystemInterface interface

    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

    FORCEINLINE int32 GetPlayerLevel() const { return Level; }
    void SetPlayerLevel(int32 NewLevel);
    void AddPlayerLevel(int32 NumLevels = 1);

    FORCEINLINE int32 GetXP() const { return CurrentXP; }
    void SetXP(int32 NewValue);
    void AddXP(int32 Value);

    void SetAttributePoints(int32 NewValue);
    void AddAttributePoints(int32 Amount);

    void SetSpellPoints(int32 NewValue);
    void AddSpellPoints(int32 Amount);


    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

    UFUNCTION()
    void OnRep_Level(int32 OldValue);

    UFUNCTION()
    void OnRep_CurrentXP(int32 OldValue);
};
