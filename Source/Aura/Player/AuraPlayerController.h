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

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> ShiftAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<class UAuraInputConfig> InputConfig;

    UPROPERTY()
    TScriptInterface<IInteractableInterface> CurrentActorUnderCursor;
    FHitResult CursorHit;

    UPROPERTY()
    mutable TObjectPtr<class UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<class UCameraOcclusionComponent> CameraOcclusionComponent;

    FVector CachedDestination = {};
    float FollowTime = 0.f;
    bool bAutoRuning = false;
    bool bMovementStarted = false;
    bool bShiftKeyPressed = false;

    UPROPERTY(EditDefaultsOnly)
    float ShortPressThreshold = 0.15f;

    UPROPERTY(EditDefaultsOnly)
    float AutoRunAcceptanceRadius = 50.f;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class USplineComponent> PathSpline;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UDamageTextComponent> DamageTextComponentClass;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<class UNiagaraSystem> ClickFX;
public:

    AAuraPlayerController();

    virtual void Tick(float DeltaTime) override;

    void AutoRun();

    UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;

    bool GetCursorHitLocation(FVector& HitLocation) const
    {
        HitLocation = CursorHit.ImpactPoint;
        return CursorHit.bBlockingHit;
    }

    UFUNCTION(Client, Reliable)
    void ClientShowDamageFloatingNumber(class APawn* TargetPawn, float Amount, bool bBlockedHit, bool bCriticalHit);

protected:

    virtual void BeginPlay() override;

    virtual void SetupInputComponent() override;

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

private:

    void Move(const struct FInputActionValue& InputValue);

    void TraceUnderCursor();

    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void SetPathToDestination();
    void AbilityInputTagHeld(FGameplayTag InputTag);

    void MakePathSpline(const TArray<FVector>& PathPoints);

    void ShiftPressed() { bShiftKeyPressed = true; }
    void ShiftReleased() { bShiftKeyPressed = false; }
};
