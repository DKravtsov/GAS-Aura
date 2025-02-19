// Copyright 4sandwiches


#include "AbilitySystem/Data/LevelUpDataAsset.h"

void ULevelUpDataAsset::PostLoad()
{
	Super::PostLoad();

	UpdateDataFromTable();
}

int32 ULevelUpDataAsset::FindLevelByXP(int32 XP) const
{
	// Levels are from [1..MaxLevel]
	check(!LevelUpData.IsEmpty());
	for (int32 Index = 1; Index < LevelUpData.Num(); ++Index)
	{
		if (XP < LevelUpData[Index].LevelUpRequirements)
			return Index;
	}
	return MaxLevel;
}

FLevelUpInfo ULevelUpDataAsset::GetLevelUpInfo(int32 InLevel) const
{
	check(!LevelUpData.IsEmpty());
	// Levels are from [1..MaxLevel]
	const int32 Index = FMath::Clamp(InLevel, 1, MaxLevel) ;
	check(LevelUpData.IsValidIndex(Index));
	return LevelUpData[Index];
}

float ULevelUpDataAsset::GetLevelPercent(int32 XP) const
{
	const int32 CurrentLevel = FindLevelByXP(XP);
	if (CurrentLevel == MaxLevel)
		return 1.f;
	const int32 CurrentRequirementXP = LevelUpData[CurrentLevel].LevelUpRequirements;
	const int32 BaseXP = LevelUpData[CurrentLevel - 1].LevelUpRequirements;
	return (CurrentRequirementXP > BaseXP) ? static_cast<float>(XP - BaseXP) / static_cast<float>(CurrentRequirementXP - BaseXP) : 0.f;
}

void ULevelUpDataAsset::UpdateDataFromTable()
{
	static const FString ContextString = TEXT("LevelUpInfo");
	if (const UDataTable* DataTable = LevelUpTable)
	{
		TArray<FLevelUpInfo*> TableRows;
		DataTable->GetAllRows(ContextString, TableRows);

		MaxLevel = TableRows.Num();
		LevelUpData.Reset(MaxLevel + 1);
		LevelUpData.Emplace(/*a dummy entry for level 0*/);
		for (const auto Row : TableRows)
		{
			LevelUpData.Add(*Row);
		}
	}
}

