// Copyright 4sandwiches


#include "LootTiersDataAsset.h"

#include "BlueprintGameplayTagLibrary.h"
#include "Game/AuraBlueprintFunctionLibrary.h"

TArray<FLootItemSpawnInfo> ULootTiersDataAsset::GetLootItemsToSpawn(AActor* SourceActor, AActor* TargetActor) const
{
	TArray<FLootItemSpawnInfo> ReturnItems;

	FGameplayTagContainer SourceTags;
	FGameplayTagContainer TargetTags;
	const int32 SourceActorLevel = UAuraBlueprintFunctionLibrary::GetCharacterLevel(SourceActor);
	const int32 TargetActorLevel = UAuraBlueprintFunctionLibrary::GetCharacterLevel(TargetActor);
	if (SourceActor)
	{
		UAuraBlueprintFunctionLibrary::GetActorOwnedGameplayTagContainer(SourceActor, SourceTags);
	}
	if (TargetActor)
	{
		UAuraBlueprintFunctionLibrary::GetActorOwnedGameplayTagContainer(TargetActor, TargetTags);
	}

	for (const FLootItem& Item : LootItemsArray)
	{
		for (int32 Index = 0; Index < Item.MaxNumberToSpawn; ++Index)
		{
			if (SourceActorLevel < Item.SourceMinLevel || TargetActorLevel < Item.TargetMinLevel)
				continue;
			if (SourceActor && !Item.SourceTagRequirements.RequirementsMet(SourceTags))
				continue;
			if (TargetActor && !Item.TargetTagRequirements.RequirementsMet(TargetTags))
				continue;
			if (FMath::FRand() < Item.ChanceToSpawn)
			{
				ReturnItems.Emplace(Item.GetLootInfo());
			}
		}
	}

	return ReturnItems;
}
