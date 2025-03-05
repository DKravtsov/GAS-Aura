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

    UPROPERTY(EditDefaultsOnly, Category = "LootTiers")
    TObjectPtr<class ULootTiersDataAsset> LootTiers;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class ULoadScreenSaveGame> LoadScreenSaveGameClass;

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UWorld> DefaultMap;

    UPROPERTY(EditDefaultsOnly)
    TMap<TSoftObjectPtr<UWorld>, FText> AvailableMaps;

    UPROPERTY(EditDefaultsOnly)
    FName DefaultPlayerStartTag = FName("LevelStart");

public:
    
    void SaveSlotData(class UMVVMLoadSlot* LoadSlot, int32 SlotIndex) const;
    ULoadScreenSaveGame* LoadSlotData(const FString& SlotName, int32 SlotIndex) const;
    ULoadScreenSaveGame* RetrieveInGameSaveData();
    void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const;

    static void DeleteLoadSlot(const FString& SlotName, int32 SlotIndex);

    FText GetDefaultMapDisplayName() const;
    FText GetMapDisplayName(const TSoftObjectPtr<UWorld>& Map);
    FText GetMapDisplayNameByAssetName(const FString& MapAssetName);
    TSoftObjectPtr<UWorld> GetWorldByName(const FString& MapAssetName) const;

    void TravelToMap(const UMVVMLoadSlot* SlotInfo) const;

    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

    void SaveWorldState(UWorld* World, const TSoftObjectPtr<UWorld>& DestinationMap = nullptr) const;
    void LoadWorldState(UWorld* World) const;

    virtual void RestartPlayer(AController* NewPlayer) override;

    void PlayerDied(ACharacter* DeadCharacter, APlayerController* VictimPC, const AActor* KillerActor);
    
protected:

    void BeginPlay() override;
};
