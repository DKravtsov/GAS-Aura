﻿// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void AddXP(int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	void NotifyLevelUp();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HideMagicCircle();


	/*  Saving progress */

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SaveProgress(FName CheckpointTag);
};
