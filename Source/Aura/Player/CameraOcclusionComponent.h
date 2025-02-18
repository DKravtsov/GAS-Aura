// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraOcclusionComponent.generated.h"

USTRUCT(BlueprintType)
struct FOccludedComponentInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UMaterialInterface*> OriginalMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionResponse> CollisionResponse = ECR_MAX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bOccluded : 1 = false;
};

/*
 * This component finds all actors that occludes the controlled character from the active camera.
 * It should be attached to PlayerController or Pawn(?)
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AURA_API UCameraOcclusionComponent : public UActorComponent
{
	GENERATED_BODY()
protected:

	UPROPERTY()
	TWeakObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY()
	TWeakObjectPtr<class UCameraComponent> CameraComponent;

	UPROPERTY()
	TWeakObjectPtr<class UCapsuleComponent> PawnCapsule;

	UPROPERTY(EditDefaultsOnly, Category="Camera Occlusion", meta = (ClampMin=0.1, AllowedClasses=2))
	float CapsuleExt;

	UPROPERTY()
	TObjectPtr<APawn> OwningPawn;

	UPROPERTY(BlueprintReadOnly)
	TMap<TObjectPtr<UStaticMeshComponent>, FOccludedComponentInfo> OccludersInfoMap;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Occlusion")
	TObjectPtr<UMaterialInterface> FadeMaterial;

public:	
	UCameraOcclusionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate(bool bReset = false) override;

protected:
	virtual void BeginPlay() override;

	void HideMesh(UStaticMeshComponent* StaticMesh);
	void ShowMesh(UStaticMeshComponent* StaticMesh, FOccludedComponentInfo& Info);

	void ShowAllMeshes();

	void InitPawn();
};
