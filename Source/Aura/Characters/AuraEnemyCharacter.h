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

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    int32 Level = 1;

public:

    AAuraEnemyCharacter();

    //~ Begin of IInteractableInterface interface
    void HighlightActor() override;
    void UnhighlightActor() override;
    //~ End of IInteractableInterface interface

    //~ Begin of ICombatInterface interface
    int32 GetCharacterLevel() const override { return Level; }
    //~ End of ICombatInterface interface

protected:

    //~ Begin APawn Interface
    void PossessedBy(AController* NewController) override;
    //~End APawn Interface

};
