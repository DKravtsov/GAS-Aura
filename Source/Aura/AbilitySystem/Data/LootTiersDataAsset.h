// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "LootTiersDataAsset.generated.h"

UENUM(BlueprintType)
enum class ESpawnLootLevelPolicy : uint8
{
	FromEnemy, /// Level of the loot item will be the same as the level of an Enemy that drops this loot
	FromItem, /// Level of the loot will be set as in the Item class
};
USTRUCT(BlueprintType)
struct FLootItemSpawnInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	ESpawnLootLevelPolicy LevelPolicy = ESpawnLootLevelPolicy::FromEnemy;

public:
	FLootItemSpawnInfo() = default;
	FLootItemSpawnInfo(TSubclassOf<AActor> InLootClass, ESpawnLootLevelPolicy InLevelPolicy)
		: LootClass(InLootClass), LevelPolicy(InLevelPolicy)
	{
	}
};

USTRUCT()
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	TSubclassOf<AActor> LootActorClass;

	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	float ChanceToSpawn = 0.f;

	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	int32 MaxNumberToSpawn = 1;

	// Tags the enemy (SourceActor) MUST or MUST NOT have to drop this loot
	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	FGameplayTagRequirements SourceTagRequirements;

	// Tags the Player (TargetActor) MUST or MUST NOT have to enemy drops this loot
	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	FGameplayTagRequirements TargetTagRequirements;

	// Required minimum level for enemy (SourceActor) to drop this loot
	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	int32 SourceMinLevel = 1;

	// Required minimum level for player (TargetActor) to enemy drops this loot
	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	int32 TargetMinLevel = 1;

	// How we decide what level will have this loot
	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	ESpawnLootLevelPolicy LevelPolicy = ESpawnLootLevelPolicy::FromEnemy;

	FLootItemSpawnInfo GetLootInfo() const
	{
		return FLootItemSpawnInfo(LootActorClass, LevelPolicy);
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

	// Both SourceActor and TargetActor can be null (undefined), in this case corresponding tags won't be checked
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<FLootItemSpawnInfo> GetLootItemsToSpawn(AActor* SourceActor, AActor* TargetActor) const;
};
