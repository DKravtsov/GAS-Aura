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

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UWorld> DefaultMap;

    UPROPERTY(EditDefaultsOnly)
    TMap<TSoftObjectPtr<UWorld>, FText> AvailableMaps;

public:
    
    void SaveSlotData(class UMVVMLoadSlot* LoadSlot, int32 SlotIndex) const;
    ULoadScreenSaveGame* LoadSlotData(const FString& SlotName, int32 SlotIndex) const;

    static void DeleteLoadSlot(const FString& SlotName, int32 SlotIndex);

    FText GetDefaultMapDisplayName() const;
    FText GetMapDisplayName(const TSoftObjectPtr<UWorld>& Map);

    void TravelToMap(const UMVVMLoadSlot* SlotInfo) const;

protected:

    void BeginPlay() override;
};
