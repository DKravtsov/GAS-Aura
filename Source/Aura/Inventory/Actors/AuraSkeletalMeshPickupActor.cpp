// Copyright 4sandwiches


#include "AuraSkeletalMeshPickupActor.h"

#include "Items/Components/InventoryHighlightableSkeletalMesh.h"


AAuraSkeletalMeshPickupActor::AAuraSkeletalMeshPickupActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(MeshComponentName, UInventoryHighlightableSkeletalMesh::StaticClass()))
{
}
