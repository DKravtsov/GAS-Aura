// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "AuraPlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerCharacter : public AAuraCharacterBase
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UCameraComponent> CameraComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UGameplayEffect> LevelUpEffect;

public:

    AAuraPlayerCharacter();

    //~ Begin APawn Interface
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    //~End APawn Interface

    //~ Begin of ICombatInterface interface
    virtual int32 BP_GetCharacterLevel_Implementation() const override;
    virtual void AddXP_Implementation(int32 Amount) override;
    virtual EAuraCharacterClass GetCharacterClass_Implementation() const override;
    virtual void NotifyLevelUp_Implementation() override;
    //~ End of ICombatInterface interface

private:
    void InitAbilitySystemComponent();
    void InitOverlay();

    mutable int32 CachedPlayerLevel = 1;
};
