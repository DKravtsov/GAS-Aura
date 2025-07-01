// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AuraPickupActor.h"
#include "AuraSkeletalMeshPickupActor.generated.h"

UCLASS()
class AAuraSkeletalMeshPickupActor : public AAuraPickupActor
{
	GENERATED_BODY()

public:

	AAuraSkeletalMeshPickupActor(const FObjectInitializer& ObjectInitializer=FObjectInitializer::Get());

};
