// Copyright 4sandwiches


#include "AuraPickupActor.h"

#include "Items/Components/InventoryItemComponent.h"

FName AAuraPickupActor::InventoryItemComponentName = "InventoryItemComponent";
FName AAuraPickupActor::MeshComponentName = "Mesh";

AAuraPickupActor::AAuraPickupActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	InventoryItemComponent = CreateDefaultSubobject<UInventoryItemComponent>(InventoryItemComponentName);

	Mesh = CreateDefaultSubobject<UMeshComponent, UStaticMeshComponent>(MeshComponentName);
	Mesh->SetupAttachment(RootComponent);
}

// Called every frame
void AAuraPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(Mesh))
	{
		Mesh->AddWorldRotation(FRotator(0.0f, RotationRate, 0.0f));
	}
}

void AAuraPickupActor::GetMoveToDestination_Implementation(FVector& OutDestination) const
{
	OutDestination = GetActorLocation();
}

