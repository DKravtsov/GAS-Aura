// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"


UENUM()
enum class ESaveSlotStatus : uint8
{
	Vacant,
	EnterName,
	Taken
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
};
