// Copyright 4sandwiches


#include "MapEntrance.h"

#include "Aura.h"
#include "Components/BoxComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerInterface.h"


AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	TargetDestinationComponent = CreateDefaultSubobject<USceneComponent>("TargetDestinationComponent");
	TargetDestinationComponent->SetupAttachment(GetRootComponent());

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	CollisionBox->SetupAttachment(TargetDestinationComponent);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldStatic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	GetStaticMeshComponent()->SetCollisionProfileName(FName("BlockAll"));
	GetStaticMeshComponent()->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_TAN);//for highlighting
}

void AMapEntrance::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		CheckpointReached();
		
		if (const auto* GM = GetWorld()->GetAuthGameMode<AAuraGameModeBase>())
		{
			GM->SaveWorldState(GetWorld(), DestinationMap);
		}
			
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}

void AMapEntrance::CheckpointReached()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ReceivedCheckpointReached();

}

void AMapEntrance::HighlightActor_Implementation()
{
	GetStaticMeshComponent()->SetRenderCustomDepth(true);
}

void AMapEntrance::UnhighlightActor_Implementation()
{
	GetStaticMeshComponent()->SetRenderCustomDepth(false);
}

void AMapEntrance::GetMoveToDestination_Implementation(FVector& OutDestination) const
{
	OutDestination = TargetDestinationComponent->GetComponentLocation();
}


