// Copyright 4sandwiches


#include "Actors/PointCollection.h"

#include "Game/AuraBlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

}

float APointCollection::GetRadius() const
{
	// Calculate the distance to the furthest point in the collection
	float SqRadius = 0.f;
	const FVector Origin = Points[0].GetLocation();
	for (const auto& Pt : Points)
	{
		const float Dist = FVector::DistSquaredXY(Pt.GetLocation(), Origin);
		if (Dist > SqRadius)
			SqRadius = Dist;
	}
	return FMath::Sqrt(SqRadius);
}

TArray<FTransform> APointCollection::GetGroundPoints(const UObject* WorldContextObject,
	TSubclassOf<APointCollection> PointCollectionClass, const FVector& GroundLocation, const int32 NumPoints, const float YawOverride)
{
	check(PointCollectionClass);
	APointCollection* Collection = PointCollectionClass->GetDefaultObject<APointCollection>();
	check(Collection && Collection->Points.Num() >= NumPoints);
	
	TArray<FTransform> Result;
	Result.Reserve(NumPoints);

	const auto& Points = Collection->Points;
	FVector Origin = GroundLocation;
	const float Radius = Collection->GetRadius();
	TArray<AActor*> IgnoreActors;
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UAuraBlueprintFunctionLibrary::GetAllLivePlayersInRadius(WorldContextObject, IgnoreActors, Radius + 100.f, Origin, {});
	for (auto Pt : Points)
	{
		if (Result.Num() >= NumPoints)
			break;

		Pt.SetLocation(GroundLocation + Pt.GetLocation());

		if (YawOverride != 0.f)
		{
			// Rotate the Pt around Origin
			FVector LocalPt = Pt.GetLocation() - Origin;
			LocalPt = LocalPt.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt.SetLocation(Origin + LocalPt);
		}

		const FVector RaisedLocation = Pt.GetLocation() + FVector(0.f, 0.f, 500.f);
		const FVector LoweredLocation = Pt.GetLocation() + FVector(0.f, 0.f, -500.f);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActors(IgnoreActors);
		if (World->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), Params))
		{
			Pt.SetLocation(HitResult.ImpactPoint);
			Pt.SetRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.Normal).Quaternion());
			Result.Add(Pt);
		}
	}

	return Result;
	
}

