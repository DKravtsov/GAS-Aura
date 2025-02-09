// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class IInteractableInterface;

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
    GENERATED_BODY()

private:

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputMappingContext> InputContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<class UAuraInputConfig> InputConfig;

    TScriptInterface<IInteractableInterface> CurrentActorUnderCursor;

    UPROPERTY()
    mutable TObjectPtr<class UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

public:

    AAuraPlayerController();

    void Tick(float DeltaTime) override;

    UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;

protected:

    void BeginPlay() override;

    void SetupInputComponent() override;

private:

    void Move(const struct FInputActionValue& InputValue);

    void TraceUnderCursor();

    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void AbilityInputTagHeld(FGameplayTag InputTag);
};
