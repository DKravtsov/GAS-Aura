// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiersDataAsset.generated.h"

UENUM(BlueprintType)
enum class ESpawnLootLevelPolicy : uint8
{
	FromEnemy, /// Level of the loot item will be the same as the level of an Enemy that drops this loot
	FromItem, /// Level of the loot will be set as in the Item class
};

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;

	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	float ChanceToSpawn = 0.f;

	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	int32 MaxNumberToSpawn = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	ESpawnLootLevelPolicy LevelPolicy = ESpawnLootLevelPolicy::FromEnemy;

	FLootItem() = default;

	FLootItem(TSubclassOf<AActor> InLootClass, ESpawnLootLevelPolicy InLevelPolicy)
		: LootClass(InLootClass), LevelPolicy(InLevelPolicy)
	{
	}

	FLootItem GetLootInfo() const
	{
		return FLootItem(LootClass, LevelPolicy);
	}
};

/**
 * 
 */
UCLASS()
class AURA_API ULootTiersDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, Category="LootTiers|Spawning")
	TArray<FLootItem> LootItemsArray;

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<FLootItem> GetLootItemsToSpawn() const;
};
