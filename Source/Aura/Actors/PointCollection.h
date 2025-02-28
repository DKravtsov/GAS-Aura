// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointCollection.generated.h"

UCLASS(Abstract, HideDropdown)
class AURA_API APointCollection : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Points", meta = (MakeEditWidget))
	TArray<FTransform> Points;

public:	
	APointCollection();

	float GetRadius() const;
	
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<FTransform> GetGroundPoints(const UObject* WorldContextObject, TSubclassOf<APointCollection> PointCollectionClass, const FVector& GroundLocation, const int32 NumPoints, const float YawOverride = 0.f);

};
