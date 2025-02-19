// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LevelUpRequirements = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AttributePointReward = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpellPointReward = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpDataAsset : public UDataAsset
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, meta = (RowType="/Script/Aura.LevelUpInfo", RequiredAssetDataTags="RowStructure=/Script/Aura.LevelUpInfo"))
	TObjectPtr<UDataTable> LevelUpTable;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FLevelUpInfo> LevelUpData;

	int32 MaxLevel;
	
public:

	//~ Begin of UObject interface
	virtual void PostLoad() override;
	//~ End of UObject interface

	UFUNCTION(BlueprintPure)
	int32 FindLevelByXP(int32 XP) const;

	UFUNCTION(BlueprintPure)
	FLevelUpInfo GetLevelUpInfo(int32 InLevel) const;

	UFUNCTION(BlueprintPure)
	int32 GetMaxLevel() const { return MaxLevel; }

protected:

	UFUNCTION(CallInEditor)
	void UpdateDataFromTable();
};
