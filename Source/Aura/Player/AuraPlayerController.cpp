// Copyright 4sandwiches


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/InteractableInterface.h"
#include "Player/Input/AuraInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "CameraOcclusionComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NiagaraFunctionLibrary.h"
#include "UI/Components/DamageTextComponent.h"
#include "GameFramework/Pawn.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;

    PathSpline = CreateDefaultSubobject<USplineComponent>("Spline");

    CameraOcclusionComponent = CreateDefaultSubobject<UCameraOcclusionComponent>("CameraOcclusionAwarenessComponent");
    CameraOcclusionComponent->SetAutoActivate(false);
    CameraOcclusionComponent->SetIsReplicated(true);
}

void AAuraPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TraceUnderCursor();

    if (bAutoRuning)
    {
        AutoRun();
    }
}

void AAuraPlayerController::AutoRun()
{
    APawn* ControlledPawn = GetPawn();
    if (IsValid(ControlledPawn))
    {
        const FVector LocationOnSpline = PathSpline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
        const FVector Direction = PathSpline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
        ControlledPawn->AddMovementInput(Direction, 1.f);

        
        const float DistanceToDestination = FVector::DistXY(LocationOnSpline, CachedDestination);
        if (DistanceToDestination <= AutoRunAcceptanceRadius)
        {
            bAutoRuning = false;
        }
    }
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent() const
{
    if (AuraAbilitySystemComponent == nullptr)
    {
        AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
    }
    return AuraAbilitySystemComponent;
}

void AAuraPlayerController::ClientShowDamageFloatingNumber_Implementation(APawn* TargetPawn, float Amount, bool bBlockedHit, bool bCriticalHit)
{
    if (IsValid(TargetPawn) && DamageTextComponentClass)
    {
        auto DamageTextComp = NewObject<UDamageTextComponent>(TargetPawn, DamageTextComponentClass);
        DamageTextComp->RegisterComponent();
        //DamageTextComp->AttachToComponent(TargetPawn->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        DamageTextComp->SetWorldLocationAndRotation(TargetPawn->GetActorLocation(), TargetPawn->GetActorRotation());
        DamageTextComp->SetDamageText(Amount, bBlockedHit, bCriticalHit);
    }
}

void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();

    checkf(InputContext, TEXT("Input Context isn't set"));

    if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        InputSubsystem->AddMappingContext(InputContext, 0);
    }

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    auto AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
    
    AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

    AuraInputComponent->BindAbilityInputAction(InputConfig, this, 
        &AAuraPlayerController::AbilityInputTagPressed, 
        &AAuraPlayerController::AbilityInputTagReleased,
        &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputValue)
{
    const FVector2D InputVector = InputValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
    const FRotationMatrix RotationMatrix(YawRotation);

    const FVector Forward = RotationMatrix.GetUnitAxis(EAxis::X);
    const FVector Right = RotationMatrix.GetUnitAxis(EAxis::Y);

    if (auto ControlledPawn = GetPawn())
    {
        ControlledPawn->AddMovementInput(Forward, InputVector.Y);
        ControlledPawn->AddMovementInput(Right, InputVector.X);
    }
}

void AAuraPlayerController::TraceUnderCursor()
{
    if (!GetHitResultUnderCursor(ECC_MouseTrace, false, CursorHit))
    {
        return;
    }

    auto LastActorUnderCursor = CurrentActorUnderCursor;
    CurrentActorUnderCursor = CursorHit.GetActor();

    if (LastActorUnderCursor.GetInterface() != CurrentActorUnderCursor.GetInterface())
    {
        if (LastActorUnderCursor != nullptr)
        {
            LastActorUnderCursor->UnhighlightActor();
        }
        if (CurrentActorUnderCursor != nullptr)
        {
            CurrentActorUnderCursor->HighlightActor();
        }
    }
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(AuraGameplayTags::InputTag_PrimaryAction))
    {
        bAutoRuning = false;
    }
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(AuraGameplayTags::InputTag_PrimaryAction))
    {
        if (CurrentActorUnderCursor == nullptr && bMovementStarted)
        {
            // we are going to run, won't try to activate any ability

            if (FollowTime <= ShortPressThreshold)
            {
                SetPathToDestination();

                UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickFX, CachedDestination);
            }

        }
        FollowTime = 0.f;
        bMovementStarted = false;
    }

    if (auto ASC = GetAuraAbilitySystemComponent())
    {
        ASC->AbilityInputReleased(InputTag);
    }
}

void AAuraPlayerController::SetPathToDestination()
{
    if (const APawn* ControlledPawn = GetPawn())
    {
        UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
        FNavLocation NavLocation;
        if (NavSystem->ProjectPointToNavigation(CachedDestination, NavLocation, FVector(0.f)))
        {
            UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), ControlledPawn->GetActorLocation(), NavLocation.Location);

            if (NavPath && !NavPath->PathPoints.IsEmpty())
            {
                // #todo: send the path finding request to the server and it should return this array of points

                MakePathSpline(NavPath->PathPoints);
                CachedDestination = NavPath->PathPoints.Last();
                bAutoRuning = true;
            }
        }
        else
        {
            bAutoRuning = false;
        }
    }
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{

    if (InputTag.MatchesTagExact(AuraGameplayTags::InputTag_PrimaryAction))
    {
        if (CurrentActorUnderCursor == nullptr && !bShiftKeyPressed)
        {
            // we are going to run, won't try to activate any ability

            FollowTime += GetWorld()->GetDeltaSeconds();

            if (CursorHit.bBlockingHit)
            {
                CachedDestination = CursorHit.ImpactPoint;
            }

            if (auto ControlledPawn = GetPawn())
            {
                const FVector WorldDir = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal2D();
                ControlledPawn->AddMovementInput(WorldDir, 1.f);
            }

            bMovementStarted = true;

            return;
        }
    }

    if (auto ASC = GetAuraAbilitySystemComponent())
    {
        ASC->AbilityInputPressed(InputTag);
    }
}

void AAuraPlayerController::MakePathSpline(const TArray<FVector>& PathPoints)
{
    PathSpline->ClearSplinePoints();
    
    for (const FVector& P : PathPoints)
    {
        PathSpline->AddSplinePoint(P, ESplineCoordinateSpace::World, false);
    }
    PathSpline->UpdateSpline();
}

void AAuraPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (CameraOcclusionComponent)
    {
        CameraOcclusionComponent->Activate();
    }
}

void AAuraPlayerController::OnUnPossess()
{
    Super::OnUnPossess();

    if (CameraOcclusionComponent)
    {
        CameraOcclusionComponent->Deactivate();
    }
}
