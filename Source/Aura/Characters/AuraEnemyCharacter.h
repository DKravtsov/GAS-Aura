// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interaction/InteractableInterface.h"
#include "AuraEnemyCharacter.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemyCharacter : public AAuraCharacterBase, public IInteractableInterface
{
    GENERATED_BODY()

public:

    AAuraEnemyCharacter();

    //~ Begin of IInteractableInterface interface
    void HighlightActor() override;
    void UnhighlightActor() override;
    //~ End of IInteractableInterface interface
};
