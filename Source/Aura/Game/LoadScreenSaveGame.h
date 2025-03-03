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
struct FAbilitySavedInfo
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
	TArray<FAbilitySavedInfo> Abilities;
};
