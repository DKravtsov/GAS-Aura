// Copyright 4sandwiches


#include "AuraStaticMeshPickupActor.h"

#include "Items/Components/InventoryHighlightableMesh.h"


AAuraStaticMeshPickupActor::AAuraStaticMeshPickupActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(MeshComponentName, UInventoryHighlightableMesh::StaticClass()))
{
}
