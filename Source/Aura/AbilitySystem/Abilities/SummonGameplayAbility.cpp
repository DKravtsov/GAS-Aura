// Copyright 4sandwiches


#include "AbilitySystem/Abilities/SummonGameplayAbility.h"

#include "NavigationSystem.h"
#include "Algo/Accumulate.h"
#include "Algo/ForEach.h"

TArray<FVector> USummonGameplayAbility::GetSpawnLocations() const
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector OwnerLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread/2.f, FVector::UpVector);
	
	TArray<FVector> Result;
	Result.Reserve(NumMinions);
	const float DeltaSpread = SpawnSpread / NumMinions;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	
	for (int Index = 0; Index < NumMinions; ++Index)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * Index, FVector::UpVector);
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
