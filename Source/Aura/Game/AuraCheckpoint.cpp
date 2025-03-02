// Copyright 4sandwiches


#include "AuraCheckpoint.h"

#include "Components/SphereComponent.h"


AAuraCheckpoint::AAuraCheckpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointSphere = CreateDefaultSubobject<USphereComponent>("CheckpointSphere");
	CheckpointSphere->SetupAttachment(GetRootComponent());
	CheckpointSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CheckpointSphere->SetCollisionObjectType(ECC_WorldStatic);
	CheckpointSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CheckpointSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionProfileName(FName("BlockAll"));
}

void AAuraCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
	CheckpointSphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraCheckpoint::OnSphereOverlap);
}

void AAuraCheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		CheckpointSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CheckpointReached(OtherActor);
	}
}

