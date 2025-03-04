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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    EAuraCharacterClass CharacterClass = EAuraCharacterClass::Warrior;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    int32 CharacterLevel = 1;

    UPROPERTY(BlueprintReadOnly, Category = Combat, Transient)
    bool bHitReacting = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
    float DeadBodyLifeSpan = 8.f;

    UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (TitleProperty = "MontageTag"))
    TArray<FAttackInfo> AttackMontages;

    UPROPERTY(BlueprintReadWrite, Category = Combat, Transient)
    TObjectPtr<AActor> CombatTarget;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<class UBehaviorTree> BehaviorTree;

    UPROPERTY()
    TObjectPtr<class AAuraAIController> AuraAIController;
    
public:

    AAuraEnemyCharacter();

    //~ Begin of IInteractableInterface interface
    virtual void HighlightActor_Implementation() override;
    virtual void UnhighlightActor_Implementation() override;
    //~ End of IInteractableInterface interface

    //~ Begin of ICombatInterface interface
    virtual int32 BP_GetCharacterLevel_Implementation() const override { return CharacterLevel; }
    virtual EAuraCharacterClass GetCharacterClass_Implementation() const override;
    virtual void SetCombatTarget_Implementation(AActor* TargetActor) override { CombatTarget = TargetActor; }
    virtual AActor* GetCombatTarget_Implementation() const override { return CombatTarget; }
    virtual void Die(AActor* KillerActor, const FVector& DeathImpulse) override;
    virtual TArray<FAttackInfo> GetAttackMontages_Implementation() const override { return AttackMontages;}
    virtual FAttackInfo GetTaggedMontageByTag_Implementation(FGameplayTag MontageTag) override;
    virtual int32 GetRewardXP_Implementation() const override;
    //~ End of ICombatInterface interface

    void HitReactTagChanged(const FGameplayTag Tag, int32 Count);

    void SetCharacterLevel(const int32 NewLevel) { CharacterLevel = NewLevel; }
    // Note: GetCharacterLevel() implemented in the base class

protected:

    virtual void BeginPlay() override;

    //~ Begin APawn Interface
    virtual void PossessedBy(AController* NewController) override;
    //~End APawn Interface

    // Must be implemented if enemy wants to display his health on the screen. This will be called during BeginPlay()
    UFUNCTION(BlueprintImplementableEvent)
    class UAuraUserWidget* GetHealthOverlayWidget() const;

    virtual void InitializeDefaultAttributes() override;
    virtual void GrantStartupAbilities() override;

    void UpdateBlackboardMinionCount() const;

    virtual void AddedMinion(APawn* NewMinion) override;
    virtual void RemovedMinion(APawn* Minion) override;
};
