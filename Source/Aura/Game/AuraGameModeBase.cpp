// Copyright 4sandwiches


#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
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
	LoadScreenSaveGame->MapName = LoadSlot->GetMap().ToString();

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

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraGameModeBase::TravelToMap(const UMVVMLoadSlot* SlotInfo) const
{
	check(SlotInfo && !SlotInfo->GetMap().IsNull());
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, SlotInfo->GetMap());
}
