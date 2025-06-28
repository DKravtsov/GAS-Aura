// Copyright 4sandwiches


#include "EquipmentManagement/ProxyMesh/InventoryProxyMeshActor.h"

#include "EquipmentManagement/Components/InventoryEquipmentComponent.h"
#include "GameFramework/Character.h"


AInventoryProxyMeshActor::AInventoryProxyMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	EquipmentComponent = CreateDefaultSubobject<UInventoryEquipmentComponent>("Equipment");
	EquipmentComponent->SetOwningSkeletalMesh(Mesh);
	EquipmentComponent->SetIsProxy(true);

	// Should we set AlwaysRelevant on this actor?
	//	No,	This actor is relevant for the local player only (or client), It's cosmetic only
}

void AInventoryProxyMeshActor::BeginPlay()
{
	Super::BeginPlay();
	DelayedInitialize();
}

void AInventoryProxyMeshActor::DelayedInitializeOwner()
{
	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController(); IsValid(PC))
		{
			if (const ACharacter* Character = PC->GetCharacter(); IsValid(Character))
			{
				if (USkeletalMeshComponent* CharacterMesh = Character->GetMesh(); IsValid(CharacterMesh))
				{
					SourceMesh = CharacterMesh;

					Mesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
					Mesh->SetAnimInstanceClass(SourceMesh->GetAnimClass());

					EquipmentComponent->InitializeOwner(PC);
				}
			}
		}
	}
	DelayedInitialize();
}

void AInventoryProxyMeshActor::DelayedInitialize()
{
	GetWorldTimerManager().SetTimerForNextTick(this, &AInventoryProxyMeshActor::DelayedInitializeOwner);
}

