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

public:

    AAuraPlayerCharacter();

    //~ Begin APawn Interface
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    //~End APawn Interface

    //~ Begin of ICombatInterface interface
    virtual int32 GetCharacterLevel() const override;
    virtual void AddXP_Implementation(int32 Amount) override;
    //~ End of ICombatInterface interface

private:
    void InitAbilitySystemComponent();
    void InitOverlay();
};
