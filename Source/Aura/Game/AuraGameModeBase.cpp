// Copyright 4sandwiches


#include "Game/AuraGameModeBase.h"

#include "AuraGameInstance.h"
#include "EngineUtils.h"
#include "Logs.h"
#include "SaveGameInterface.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVMLoadSlot.h"
#include "UI/Widgets/LoadScreenWidget.h"

void AAuraGameModeBase::SaveSlotData(class UMVVMLoadSlot* LoadSlot, int32 SlotIndex) const
{
	// first, delete the slot if already exists
	DeleteLoadSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
	
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotName = LoadSlot->GetLoadSlotName();
	LoadScreenSaveGame->SlotIndex = SlotIndex;
	LoadScreenSaveGame->SlotStatus = ESaveSlotStatus::Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapAssetName();
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::LoadSlotData(const FString& SlotName, int32 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		return Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	return Cast<ULoadScreenSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData()
{
	UAuraGameInstance* GameInstance = GetWorld()->GetGameInstance<UAuraGameInstance>();
	check(GameInstance);

	return LoadSlotData(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const
{
	UAuraGameInstance* GameInstance = GetWorld()->GetGameInstance<UAuraGameInstance>();
	check(GameInstance);
	GameInstance->PlayerStartTag = SaveObject->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(SaveObject, GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
}

void AAuraGameModeBase::DeleteLoadSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

FText AAuraGameModeBase::GetDefaultMapDisplayName() const
{
	return AvailableMaps.FindChecked(DefaultMap);
}

FText AAuraGameModeBase::GetMapDisplayName(const TSoftObjectPtr<UWorld>& Map)
{
	if (const FText* Name = AvailableMaps.Find(Map))
	{
		return *Name;
	}
	return FText::GetEmpty();
}

FText AAuraGameModeBase::GetMapDisplayNameByAssetName(const FString& MapAssetName)
{
	for (const auto& [Map, MapDisplayName] : AvailableMaps)
	{
		if (Map.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return MapDisplayName;
		}
	}
	return FText::GetEmpty();
}

TSoftObjectPtr<UWorld> AAuraGameModeBase::GetWorldByName(const FString& MapAssetName) const
{
	for (const auto& [Map, Unused] : AvailableMaps)
	{
		if (Map.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map;
		}
	}
	return nullptr;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraGameModeBase::TravelToMap(const UMVVMLoadSlot* SlotInfo) const
{
	check(SlotInfo && !SlotInfo->GetMapAssetName().IsEmpty());
	if (const auto NewMap = GetWorldByName(SlotInfo->GetMapAssetName()); !NewMap.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, NewMap);
	}
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* BestPlayerStart = nullptr;
	FName PlayerStartTag = GetGameInstance<UAuraGameInstance>()->PlayerStartTag;
	for (TActorIterator<APlayerStart> It(GetWorld(), APlayerStart::StaticClass()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;
		if (BestPlayerStart == nullptr)
		{
			BestPlayerStart = PlayerStart; // just in case we won't find anything
		}

		if (PlayerStart->PlayerStartTag == PlayerStartTag)
		{
			BestPlayerStart = PlayerStart;
			break;
		}
	}
	return BestPlayerStart;
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const TSoftObjectPtr<UWorld>& DestinationMap) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = CastChecked<UAuraGameInstance>(GetGameInstance());

	if (auto SaveGame = LoadSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		if (!DestinationMap.IsNull())
		{
			SaveGame->MapName = DestinationMap.ToSoftObjectPath().GetAssetName();
		}
		
		if (!SaveGame->HasMap(WorldName))
		{
			SaveGame->SavedMaps.Emplace(WorldName);
		}

		FMapSavedData& SavedMap = SaveGame->GetSavedMapMutable(WorldName);
		SavedMap.SavedActors.Reset(); // clear it out, we'll fill it in with actors

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveGameInterface>())
				continue;

			FActorSavedData& SavedActor = SavedMap.SavedActors.Emplace_GetRef(Actor->GetFName());
			if (Actor->IsRootComponentMovable())
			{
				SavedActor.Transform = Actor->GetActorTransform();
			}

			FMemoryWriter Writer(SavedActor.SerializedData);
			FObjectAndNameAsStringProxyArchive Ar(Writer, true);
			Ar.ArIsSaveGame = true;
			Actor->Serialize(Ar);
		}

		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = CastChecked<UAuraGameInstance>(GetGameInstance());

	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		auto SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogMyGame, Error, TEXT("Fail to load from game slot [%s]"), *AuraGameInstance->LoadSlotName);
			return;
		}
		if (!SaveGame->HasMap(WorldName))
		{
			return;
		}
		const auto& SavedMap = SaveGame->GetSavedMap(WorldName);
		
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveGameInterface>())
				continue;

			for (const auto& SavedActor : SavedMap.SavedActors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					if (ISaveGameInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}

					FMemoryReader Reader(SavedActor.SerializedData);
					FObjectAndNameAsStringProxyArchive Ar(Reader, true);
					Ar.ArIsSaveGame = true;
					Actor->Serialize(Ar);

					ISaveGameInterface::Execute_LoadedFromSaveGame(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	LoadWorldState(GetWorld());
}
