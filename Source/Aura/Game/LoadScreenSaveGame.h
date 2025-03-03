// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"


UENUM()
enum class ESaveSlotStatus : uint8
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FAbilitySavedData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag AbilityTag;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag StatusTag;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag InputTag;

	UPROPERTY(BlueprintReadWrite)
	int32 AbilityLevel;
};

USTRUCT(BlueprintType)
struct FActorSavedData
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName;

	UPROPERTY()
	FTransform Transform;

	// Serialized properties of the Actor marked as UPROPERTY(SaveGame)
	UPROPERTY()
	TArray<uint8> SerializedData;

	
	FActorSavedData(FName InActorName = NAME_None) : ActorName(InActorName) {}

	bool operator==(const FActorSavedData& Other) const
	{
		return ActorName == Other.ActorName;
	}
};

USTRUCT(BlueprintType)
struct FMapSavedData
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName;

	UPROPERTY()
	TArray<FActorSavedData> SavedActors;

	
	FMapSavedData(const FString& InMapName = FString()) : MapAssetName(InMapName) {}

	bool operator==(const FMapSavedData& Other) const
	{
		return Other.MapAssetName.Equals(MapAssetName, ESearchCase::IgnoreCase);
	}
};

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString SlotName;

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	ESaveSlotStatus SlotStatus = ESaveSlotStatus::Vacant;

	UPROPERTY()
	FString PlayerName = TEXT("Default Name");

	UPROPERTY()
	FString MapName;

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	/* The PLayer state */

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	float Strength = 0.f;

	UPROPERTY()
	float Intelligence = 0.f;

	UPROPERTY()
	float Resilience = 0.f;
	
	UPROPERTY()
	float Vigor = 0.f;

	/* Abilities */

	UPROPERTY()
	TArray<FAbilitySavedData> Abilities;

	UPROPERTY()
	TArray<FMapSavedData> SavedMaps;

	bool HasMap(const FString& InMapName) const;
	const FMapSavedData& GetSavedMap(const FString& InMapName) const;
	FMapSavedData& GetSavedMapMutable(const FString& InMapName);
};
