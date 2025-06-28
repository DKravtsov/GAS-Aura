// Copyright 4sandwiches


#include "EquipmentManagement/EquipActor/InventoryEquipActor.h"



AInventoryEquipActor::AInventoryEquipActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}


void AInventoryEquipActor::BeginPlay()
{
	Super::BeginPlay();
	
}

