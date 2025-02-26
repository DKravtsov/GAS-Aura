// Copyright 4sandwiches


#include "AbilitySystem/Abilities/SummonGameplayAbility.h"

#include "NavigationSystem.h"
#include "Algo/Accumulate.h"
#include "Algo/ForEach.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

TArray<FVector> USummonGameplayAbility::GetSpawnLocations() const
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector OwnerLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FVector> Directions = UAuraBlueprintFunctionLibrary::GetUniformSpreadOfDirections(Forward, SpawnSpread, NumMinions);
	TArray<FVector> Result;
	Result.Reserve(Directions.Num());
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	for (const FVector& Direction : Directions)
	{
		const FVector SpawnLocation = OwnerLocation + Direction  * FMath::FRandRange(MinMaxSpawnDistance.X, MinMaxSpawnDistance.Y);
		if (FNavLocation NavLocation; NavSystem->ProjectPointToNavigation(SpawnLocation, NavLocation, FVector(.0f, 0.f, 400.f)))
		{
			Result.Add(NavLocation.Location);
		}
	}
	return Result;
}

TSubclassOf<APawn> USummonGameplayAbility::GetRandomMinionClass() const
{
	check(!MinionClasses.IsEmpty());
	
	if (!bInitialized)
	{
		MinionClasses.GetKeys(MinionClassesArray);
		MinionClasses.GenerateValueArray(Weights);
		const float Sum = Algo::Accumulate(Weights, 0.f);
		if (Sum <= 0.f)
		{
			return MinionClassesArray[0];
		}
		Algo::ForEach(Weights, [Sum](float& X) { X /= Sum; });
		bInitialized = true;
	}

	const int32 NumElements = MinionClassesArray.Num();
	float R = FMath::FRand();
	int32 Index = 0;
	while (R > Weights[Index])
	{
		R -= Weights[Index];
		++Index;
	}
	check(Index < NumElements);
	return MinionClassesArray[Index];
}
