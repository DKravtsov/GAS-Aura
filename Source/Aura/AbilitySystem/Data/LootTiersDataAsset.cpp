// Copyright 4sandwiches


#include "LootTiersDataAsset.h"

TArray<FLootItem> ULootTiersDataAsset::GetLootItemsToSpawn() const
{
	TArray<FLootItem> ReturnItems;

	for (const FLootItem& Item : LootItemsArray)
	{
		for (int32 Index = 0; Index < Item.MaxNumberToSpawn; ++Index)
		{
			if (FMath::FRand() < Item.ChanceToSpawn)
			{
				ReturnItems.Emplace(Item.GetLootInfo());
			}
		}
	}

	return ReturnItems;
}
