// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, Category = "CharacterClassDefaults")
    TObjectPtr<class UCharacterClassInfo> CharacterClassInfo;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TObjectPtr<class UAbilityInfoDataAsset> AbilityInfo;

};
