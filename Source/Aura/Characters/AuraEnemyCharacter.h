// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interaction/InteractableInterface.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraEnemyCharacter.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemyCharacter : public AAuraCharacterBase, public IInteractableInterface
{
    GENERATED_BODY()

public:

    // Will be called when either Heath or MaxHealth attributes changed
    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedSignature OnHealthAttributesChanged;

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

    void BeginPlay() override;

    //~ Begin APawn Interface
    void PossessedBy(AController* NewController) override;
    //~End APawn Interface

    // Must be implemented if enemy wants to display his health on the screen. This will be called during BeginPlay()
    UFUNCTION(BlueprintImplementableEvent)
    class UAuraUserWidget* GetHealthOverlayWidget() const;
};
