// Copyright 4sandwiches


#include "Actors/AuraEnemySpawnVolume.h"

#include "AuraEnemySpawnPoint.h"
#include "Components/ShapeComponent.h"
#include "Player/PlayerInterface.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCollisionComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCollisionComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadedFromSaveGame_Implementation()
{
	if (bReached)
	{
		SetActorEnableCollision(false);
	}
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnOverlap);
}

void AAuraEnemySpawnVolume::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		CheckpointReached();
		Destroy();
	}
}

void AAuraEnemySpawnVolume::CheckpointReached()
{
	SetActorEnableCollision(false);
	
	ReceivedCheckpointReached();
	bReached = true;
	for (const auto SpawnPoint : SpawnPoints)
	{
		SpawnPoint->SpawnEnemy();
	}
}
