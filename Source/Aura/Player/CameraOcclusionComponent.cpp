// Copyright 4sandwiches


#include "Player/CameraOcclusionComponent.h"

#include "Aura.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

int32 GDebugTraces = 0;
FAutoConsoleVariableRef CVar_DebugTraces(TEXT("CameraOcclusionDebugTraces"), GDebugTraces, TEXT("Show/Hide traces from camera occlusion test"));

UCameraOcclusionComponent::UCameraOcclusionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	CapsuleExt = 1.f;
}

void UCameraOcclusionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitPawn();
}

void UCameraOcclusionComponent::InitPawn()
{
	OwningPawn = GetOwner<APawn>();
	if (!IsValid(OwningPawn))
	{
		if (AController* Controller = GetOwner<AController>())
		{
			OwningPawn = Controller->GetPawn();
		}
	}
	if (IsValid(OwningPawn))
	{
		CameraBoom = OwningPawn->GetComponentByClass<USpringArmComponent>();
		CameraComponent = OwningPawn->GetComponentByClass<UCameraComponent>();
		PawnCapsule = OwningPawn->GetComponentByClass<UCapsuleComponent>();
	}
}

void UCameraOcclusionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//LOG_NETFUNCTIONCALL_COMPONENT;
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
		return;
	
	if (CameraBoom != nullptr && CameraBoom->bDoCollisionTest)
	{
		ShowAllMeshes();
		return;
	}

	check(CameraComponent.IsValid());
	check(PawnCapsule.IsValid());

	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = OwningPawn->GetActorLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes;
	CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore = IgnoreActors;
	ActorsToIgnore.Add(OwningPawn);
	if (const auto Movement = OwningPawn->GetComponentByClass<UCharacterMovementComponent>())
	{
		ActorsToIgnore.Add(Movement->CurrentFloor.HitResult.GetActor());
	}
	TArray<FHitResult> OutHits;

	auto ShouldDebug = GDebugTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bHit = UKismetSystemLibrary::CapsuleTraceMultiForObjects(
	  GetWorld(), Start, End, PawnCapsule->GetScaledCapsuleRadius() * CapsuleExt,
	  PawnCapsule->GetScaledCapsuleHalfHeight() * CapsuleExt, CollisionObjectTypes, true,
	  ActorsToIgnore,  ShouldDebug, OutHits, true);

	if (bHit)
	{
		TArray<const UPrimitiveComponent*> OccludedComponents;

		for (FHitResult Hit : OutHits)
		{
			if (UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(Hit.GetComponent()))
			{
				HideMesh(SM);
				OccludedComponents.Add(Hit.GetComponent());
			}
		}

		for (auto& [StaticMesh, Info] : OccludersInfoMap)
		{
			if (!OccludedComponents.Contains(StaticMesh) && Info.bOccluded)
			{
				ShowMesh(StaticMesh, Info);
				// remove visible components?
			}
		}
	}
	else
	{
		ShowAllMeshes();
	}
	
}

void UCameraOcclusionComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (IsActive())
	{
		InitPawn();

		if (!IsValid(OwningPawn) || !CameraComponent.IsValid() || !PawnCapsule.IsValid())
		{
			Deactivate();
		}
	}
}

void UCameraOcclusionComponent::HideMesh(UStaticMeshComponent* StaticMesh)
{
	FOccludedComponentInfo& Info = OccludersInfoMap.FindOrAdd(StaticMesh);
	if (!Info.bOccluded)
	{
		if (Info.OriginalMaterials.IsEmpty())
		{
			Info.OriginalMaterials = StaticMesh->GetMaterials();
		}
		
		Info.bOccluded = true;
		Info.CollisionResponse = StaticMesh->GetCollisionResponseToChannel(ECC_MouseTrace);
		StaticMesh->SetCollisionResponseToChannel(ECC_MouseTrace, ECR_Ignore);

		if (FadeMaterial)
		{
			for (int32 Index = 0; Index < Info.OriginalMaterials.Num(); ++Index)
			{
				StaticMesh->SetMaterial(Index, FadeMaterial);
			}
		}
		else
		{
			StaticMesh->SetHiddenInGame(true);
		}
	}
}

void UCameraOcclusionComponent::ShowMesh(UStaticMeshComponent* StaticMesh, FOccludedComponentInfo& Info)
{
	if (!IsValid(StaticMesh) || !IsValid(StaticMesh->GetOwner()))
	{
		// remove ?
		return;
	}
	Info.bOccluded = false;
	StaticMesh->SetCollisionResponseToChannel(ECC_MouseTrace, Info.CollisionResponse);

	if (FadeMaterial)
	{
		for (int Index = 0; Index < Info.OriginalMaterials.Num(); ++Index)
		{
			StaticMesh->SetMaterial(Index, Info.OriginalMaterials[Index]);
		}
	}
	else
	{
		StaticMesh->SetHiddenInGame(false);
	}
}

void UCameraOcclusionComponent::ShowAllMeshes()
{
	for (auto& [StaticMesh, Info] : OccludersInfoMap)
	{
		ShowMesh(StaticMesh, Info);
	}
}