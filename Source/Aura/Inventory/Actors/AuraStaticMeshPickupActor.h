// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AuraPickupActor.h"
#include "AuraStaticMeshPickupActor.generated.h"

UCLASS()
class AAuraStaticMeshPickupActor : public AAuraPickupActor
{
	GENERATED_BODY()

public:

	AAuraStaticMeshPickupActor(const FObjectInitializer& ObjectInitializer=FObjectInitializer::Get());
	
};
