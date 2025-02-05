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
    void PossessedBy(AController* NewController) override;
    void OnRep_PlayerState() override;
    //~End APawn Interface

private:
    void InitAbilitySystemComponent();
};
