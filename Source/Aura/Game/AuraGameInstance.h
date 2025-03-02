// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TObjectPtr<class UAbilityInfoDataAsset> AbilityInfo;

    UPROPERTY()
    FName PlayerStartTag;

    UPROPERTY()
    FString LoadSlotName;

    UPROPERTY()
    int32 LoadSlotIndex = 0;
};
