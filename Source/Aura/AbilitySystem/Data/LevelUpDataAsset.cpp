// Copyright 4sandwiches


#include "AbilitySystem/Data/LevelUpDataAsset.h"

void ULevelUpDataAsset::PostLoad()
{
	Super::PostLoad();

	UpdateDataFromTable();
}

int32 ULevelUpDataAsset::FindLevelByXP(int32 XP) const
{
	check(!LevelUpData.IsEmpty());
	for (int32 Index = 0; Index < LevelUpData.Num(); ++Index)
	{
		if (XP < LevelUpData[Index].LevelUpRequirements)
			return Index + 1;
	}
	return MaxLevel;
}

FLevelUpInfo ULevelUpDataAsset::GetLevelUpInfo(int32 InLevel) const
{
	check(!LevelUpData.IsEmpty());
	const int32 Index = FMath::Clamp(InLevel, 1, MaxLevel) - 1;
	return LevelUpData[Index];
}

void ULevelUpDataAsset::UpdateDataFromTable()
{
	static const FString ContextString = TEXT("LevelUpInfo");
	if (const UDataTable* DataTable = LevelUpTable)
	{
		TArray<FLevelUpInfo*> TableRows;
		DataTable->GetAllRows(ContextString, TableRows);

		MaxLevel = TableRows.Num();
		LevelUpData.Reset(MaxLevel);
		for (const auto Row : TableRows)
		{
			LevelUpData.Add(*Row);
		}
	}
}

