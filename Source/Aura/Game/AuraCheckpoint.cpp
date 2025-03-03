// Copyright 4sandwiches


#include "AuraCheckpoint.h"

#include "AuraGameModeBase.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerInterface.h"


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

void AAuraCheckpoint::LoadedFromSaveGame_Implementation()
{
	if (bCheckpointReached)
	{
		CheckpointReached();
	}
}

void AAuraCheckpoint::SetCheckpointReached(bool bNewValue)
{
	if (bCheckpointReached != bNewValue)
	{
		bCheckpointReached = bNewValue;
		CheckpointReached();
	}
}

void AAuraCheckpoint::CheckpointReached()
{
	if (bCheckpointReached)
	{
		CheckpointSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		CheckpointSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	ReceivedCheckpointReached();
}

void AAuraCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
	CheckpointSphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraCheckpoint::OnSphereOverlap);
}

void AAuraCheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		SetCheckpointReached(true);

		if (const auto* GM = GetWorld()->GetAuthGameMode<AAuraGameModeBase>())
		{
			GM->SaveWorldState(GetWorld());
		}
		
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
	}
}

