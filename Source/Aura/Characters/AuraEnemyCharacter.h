// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Characters/AuraCharacterBase.h"
#include "Interaction/InteractableInterface.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
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
    EAuraCharacterClass CharacterClass = EAuraCharacterClass::Warrior;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    int32 CharacterLevel = 1;

    UPROPERTY(BlueprintReadOnly, Category = Combat, Transient)
    bool bHitReacting = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
    float DeadBodyLifeSpan = 8.f;

    UPROPERTY(BlueprintReadWrite, Category = Combat)
    TObjectPtr<AActor> CombatTarget;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<class UBehaviorTree> BehaviorTree;

    UPROPERTY()
    TObjectPtr<class AAuraAIController> AuraAIController;
    
public:

    AAuraEnemyCharacter();

    //~ Begin of IInteractableInterface interface
    void HighlightActor() override;
    void UnhighlightActor() override;
    //~ End of IInteractableInterface interface

    //~ Begin of ICombatInterface interface
    virtual int32 GetCharacterLevel() const override { return CharacterLevel; }
    virtual void SetCombatTarget_Implementation(AActor* TargetActor) override { CombatTarget = TargetActor; }
    virtual AActor* GetCombatTarget_Implementation() const override { return CombatTarget; }
    virtual void Die() override;
    //~ End of ICombatInterface interface

    void HitReactTagChanged(const FGameplayTag Tag, int32 Count);

protected:

    void BeginPlay() override;

    //~ Begin APawn Interface
    void PossessedBy(AController* NewController) override;
    //~End APawn Interface

    // Must be implemented if enemy wants to display his health on the screen. This will be called during BeginPlay()
    UFUNCTION(BlueprintImplementableEvent)
    class UAuraUserWidget* GetHealthOverlayWidget() const;

    void InitializeDefaultAttributes() override;
    void GrantStartupAbilities() override;
};
