// Copyright 4sandwiches


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/InteractableInterface.h"
#include "Player/Input/AuraInputComponent.h"
#include "Player/InventoryPlayerControllerComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "CameraOcclusionComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/MagicCircle.h"
#include "Characters/CombatInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Components/DamageTextComponent.h"
#include "GameFramework/Pawn.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;

    PathSpline = CreateDefaultSubobject<USplineComponent>("Spline");

    CameraOcclusionComponent = CreateDefaultSubobject<UCameraOcclusionComponent>("CameraOcclusionAwarenessComponent");
    CameraOcclusionComponent->SetAutoActivate(false);
    CameraOcclusionComponent->SetIsReplicated(true);

    InventoryControllerComponent = CreateDefaultSubobject<UInventoryPlayerControllerComponent>("InventoryControllerComponent");
}

void AAuraPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TraceUnderCursor();
    UpdateMagicCircleLocation();

    if (bAutoRunning)
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
            bAutoRunning = false;
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

    InventoryControllerComponent->SetupInputComponent(AuraInputComponent);
}

void AAuraPlayerController::Move(const FInputActionValue& InputValue)
{
    auto ASC = GetAuraAbilitySystemComponent();
    if ( ASC && ASC->HasMatchingGameplayTag (AuraGameplayTags::Player_Block_Input))
    {
        return;
    }
    if (const auto ControlledPawn = GetPawn())
    {
        const FVector2D InputVector = InputValue.Get<FVector2D>();
        const FRotator Rotation = ControlledPawn->GetBaseAimRotation();
        const FRotationMatrix RotationMatrix(FRotator(0.f, Rotation.Yaw, 0.f));

        const FVector Forward = RotationMatrix.GetUnitAxis(EAxis::X);
        const FVector Right = RotationMatrix.GetUnitAxis(EAxis::Y);

        ControlledPawn->AddMovementInput(Forward, InputVector.Y);
        ControlledPawn->AddMovementInput(Right, InputVector.X);
    }
}

void AAuraPlayerController::TraceUnderCursor()
{
    auto ASC = GetAuraAbilitySystemComponent();
    if ( ASC && ASC->HasMatchingGameplayTag (AuraGameplayTags::Player_Block_CursorTrace))
    {
        if (CurrentActorUnderCursor != nullptr)
        {
            IInteractableInterface::Execute_UnhighlightActor(CurrentActorUnderCursor);
            CurrentActorUnderCursor = nullptr;
            TargetingStatus = ETargetingStatus::None;
        }
        return;
    }

    const auto TraceChannel = MagicCircle ? ECC_MagicCircleTrace : ECC_MouseTrace;
    
    if (!GetHitResultUnderCursor(TraceChannel, false, CursorHit))
    {
        InventoryControllerComponent->UpdateInteractionTraceResult(nullptr);
        return;
    }

    AActor* LastActorUnderCursor = CurrentActorUnderCursor;
    {
        auto HitActor = CursorHit.GetActor();
        CurrentActorUnderCursor = (HitActor && HitActor->Implements<UInteractableInterface>()) ? HitActor : nullptr;
    }

    if (LastActorUnderCursor != CurrentActorUnderCursor)
    {
        if (LastActorUnderCursor != nullptr)
        {
            IInteractableInterface::Execute_UnhighlightActor(LastActorUnderCursor);
        }
        if (CurrentActorUnderCursor != nullptr)
        {
            IInteractableInterface::Execute_HighlightActor(CurrentActorUnderCursor);

            if (CurrentActorUnderCursor->Implements<UCombatInterface>())
            {
                TargetingStatus = ETargetingStatus::Enemy;
            }
            else
            {
                TargetingStatus = ETargetingStatus::Interactable;
            }
        }
        else
        {
            TargetingStatus = ETargetingStatus::None;
        }
    }

    // Note: This requires that interactable items also implement IInteractableInterface
    InventoryControllerComponent->UpdateInteractionTraceResult(CurrentActorUnderCursor);
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    auto ASC = GetAuraAbilitySystemComponent();
    if ( ASC && ASC->HasMatchingGameplayTag (AuraGameplayTags::Player_Block_Input))
    {
        return;
    }
    if (InputTag.MatchesTagExact(AuraGameplayTags::InputTag_PrimaryAction))
    {
        bAutoRunning = false;
    }
    
    if (ASC)
    {
        ASC->AbilityInputPressed(InputTag);
    }
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    auto ASC = GetAuraAbilitySystemComponent();
    if ( ASC && ASC->HasMatchingGameplayTag (AuraGameplayTags::Player_Block_Input))
    {
        return;
    }
    if (InputTag.MatchesTagExact(AuraGameplayTags::InputTag_PrimaryAction))
    {
        if (TargetingStatus != ETargetingStatus::Enemy && bMovementStarted)
        {
            // we are going to run, won't try to activate any ability

            if (FollowTime <= ShortPressThreshold && GetCharacter()->GetCharacterMovement()->MovementMode != MOVE_None)
            {
                SetPathToDestination();

                UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickFX, CachedDestination);
            }

        }
        FollowTime = 0.f;
        bMovementStarted = false;
    }

    if (ASC)
    {
        ASC->AbilityInputReleased(InputTag);
    }
}

void AAuraPlayerController::SetPathToDestination()
{
    if (const APawn* ControlledPawn = GetPawn())
    {
        if (TargetingStatus == ETargetingStatus::Interactable)
        {
            check(CurrentActorUnderCursor);
            IInteractableInterface::Execute_GetMoveToDestination(CurrentActorUnderCursor, CachedDestination);
        }
        
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
                bAutoRunning = true;
            }
        }
        else
        {
            bAutoRunning = false;
        }
    }
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    auto ASC = GetAuraAbilitySystemComponent();
    if ( ASC && ASC->HasMatchingGameplayTag (AuraGameplayTags::Player_Block_Input))
    {
        return;
    }

    if (InputTag.MatchesTagExact(AuraGameplayTags::InputTag_PrimaryAction))
    {
        if (TargetingStatus != ETargetingStatus::Enemy && !bShiftKeyPressed)
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

    if (ASC)
    {
        ASC->AbilityInputHeld(InputTag);
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

void AAuraPlayerController::UpdateMagicCircleLocation()
{
    if (IsValid(MagicCircle))
    {
        MagicCircle->SetActorLocation(CursorHit.Location);
    }
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

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
    ClientShowMagicCircle(DecalMaterial);
}

void AAuraPlayerController::HideMagicCircle()
{
    ClientHideMagicCircle();
}

void AAuraPlayerController::ClientShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
    if (!IsValid(MagicCircle))
    {
        check(MagicCircleClass != nullptr);
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass, SpawnParams);
        MagicCircle->SetReplicates(false);
        if (DecalMaterial != nullptr)
        {
            MagicCircle->SetDecalMaterial(DecalMaterial);
        }
    }
}

void AAuraPlayerController::ClientHideMagicCircle_Implementation()
{
    if (IsValid(MagicCircle))
    {
        MagicCircle->Destroy();
        MagicCircle = nullptr;
    }
}
