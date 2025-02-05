// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

    TScriptInterface<IInteractableInterface> CurrentActorUnderCursor;

public:

    AAuraPlayerController();

    void Tick(float DeltaTime) override;

protected:

    void BeginPlay() override;

    void SetupInputComponent() override;

private:

    void Move(const struct FInputActionValue& InputValue);

    void TraceUnderCursor();
};
