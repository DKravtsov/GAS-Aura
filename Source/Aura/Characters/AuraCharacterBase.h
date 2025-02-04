// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, Category = Combat)
    TObjectPtr<class USkeletalMeshComponent> Weapon;

public:

    AAuraCharacterBase();

protected:

    virtual void BeginPlay() override;

};
