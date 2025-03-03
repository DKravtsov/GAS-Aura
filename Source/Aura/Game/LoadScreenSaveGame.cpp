// Copyright 4sandwiches


#include "Game/LoadScreenSaveGame.h"

bool ULoadScreenSaveGame::HasMap(const FString& InMapName) const
{
	return SavedMaps.ContainsByPredicate(
		[InMapName](const FMapSavedData& MapData)
		{
			return MapData.MapAssetName.Equals(InMapName, ESearchCase::IgnoreCase);
		});
}

const FMapSavedData& ULoadScreenSaveGame::GetSavedMap(const FString& InMapName) const
{
	for (const auto& MapData : SavedMaps)
	{
		if (MapData.MapAssetName.Equals(InMapName, ESearchCase::IgnoreCase))
		{
			return MapData;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Map [%s] doesn't exist in saved data."), *InMapName);

	// HasMap() should be always used to make sure the map exists in the saved data
	checkNoEntry();

	static FMapSavedData EmptyMapData;
	return EmptyMapData;
}

FMapSavedData& ULoadScreenSaveGame::GetSavedMapMutable(const FString& InMapName)
{
	for (auto& MapData : SavedMaps)
	{
		if (MapData.MapAssetName.Equals(InMapName, ESearchCase::IgnoreCase))
		{
			return MapData;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Map [%s] doesn't exist in saved data."), *InMapName);
	// HasMap() should be always used to make sure the map exists in the saved data
	checkNoEntry();
	static FMapSavedData EmptyMapData;
	return EmptyMapData;
}
