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

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class ULoadScreenSaveGame> LoadScreenSaveGameClass;

    void SaveSlotData(class UMVVMLoadSlot* LoadSlot, int32 SlotIndex) const;
    ULoadScreenSaveGame* LoadSlotData(const FString& SlotName, int32 SlotIndex) const;
};
