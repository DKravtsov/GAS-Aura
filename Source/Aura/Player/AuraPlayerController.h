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

    UPROPERTY()
    TScriptInterface<IInteractableInterface> CurrentActorUnderCursor;
    FHitResult CursorHit;

    UPROPERTY()
    mutable TObjectPtr<class UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

    FVector CachedDestination = {};
    float FollowTime = 0.f;
    bool bAutoRuning = false;

    UPROPERTY(EditDefaultsOnly)
    float ShortPressThreshold = 0.15f;

    UPROPERTY(EditDefaultsOnly)
    float AutoRunAcceptanceRadius = 50.f;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class USplineComponent> PathSpline;


public:

    AAuraPlayerController();

    void Tick(float DeltaTime) override;

    void AutoRun();

    UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;

protected:

    void BeginPlay() override;

    void SetupInputComponent() override;

private:

    void Move(const struct FInputActionValue& InputValue);

    void TraceUnderCursor();

    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void SetPathToDestination();
    void AbilityInputTagHeld(FGameplayTag InputTag);

    void MakePathSpline(const TArray<FVector>& PathPoints);
};
