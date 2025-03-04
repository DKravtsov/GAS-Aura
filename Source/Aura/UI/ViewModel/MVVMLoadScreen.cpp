// Copyright 4sandwiches


#include "UI/ViewModel/MVVMLoadScreen.h"

#include "MVVMLoadSlot.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UMVVMLoadScreen::InitializeLoadSlots()
{
	for (int32 Index = 0; Index < NumLoadSlots; ++Index)
	{
		LoadSlots[Index] = NewObject<UMVVMLoadSlot>(this, LoadSlotViewModelClass);
		LoadSlots[Index]->SetLoadSlotName(FString::Printf(TEXT("LoadSlot_%d"), Index));
		LoadSlots[Index]->SlotIndex = Index;
	}
	OnSlotSelectionCleared.Broadcast();
}

UMVVMLoadSlot* UMVVMLoadScreen::GetLoadSlot(int32 Index) const
{
	checkf(Index >= 0 && Index < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);
	return LoadSlots[Index];
}

void UMVVMLoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	checkf(Slot >= 0 && Slot < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	LoadSlots[Slot]->SetMap(AuraGameMode->DefaultMap);
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SlotStatus = ESaveSlotStatus::Taken;
	LoadSlots[Slot]->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
	LoadSlots[Slot]->SetPlayerLevel(1);

	AuraGameMode->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();
}

void UMVVMLoadScreen::NewGameButtonPressed(int32 Slot)
{
	checkf(Slot >= 0 && Slot < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);
	LoadSlots[Slot]->OnSetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVMLoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	checkf(Slot >= 0 && Slot < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);

	SetSelectedSlotIndex(Slot);
	
	OnSlotSelected.Broadcast();
	
	for (int Index = 0; Index < NumLoadSlots; ++Index)
	{
		LoadSlots[Index]->OnSetEnableSlot.Broadcast(Index != Slot);
	}
}

void UMVVMLoadScreen::PlayFromSelectedSlot()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	UAuraGameInstance* AuraGameInstance = AuraGameMode->GetGameInstance<UAuraGameInstance>();
	AuraGameInstance->LoadSlotName = LoadSlots[SelectedSlotIndex]->GetLoadSlotName();
	AuraGameInstance->LoadSlotIndex = SelectedSlotIndex;
	AuraGameInstance->PlayerStartTag = LoadSlots[SelectedSlotIndex]->PlayerStartTag;

	AuraGameMode->TravelToMap(LoadSlots[SelectedSlotIndex]);
}

void UMVVMLoadScreen::DeleteSelectedSlot()
{
	if (SelectedSlotIndex == INDEX_NONE)
		return;
	AAuraGameModeBase::DeleteLoadSlot(LoadSlots[SelectedSlotIndex]->GetLoadSlotName(), SelectedSlotIndex);
	LoadSlots[SelectedSlotIndex]->SlotStatus = ESaveSlotStatus::Vacant;
	LoadSlots[SelectedSlotIndex]->InitializeSlot();
	LoadSlots[SelectedSlotIndex]->OnSetEnableSlot.Broadcast(true);
	SetSelectedSlotIndex(INDEX_NONE);
	OnSlotSelectionCleared.Broadcast();
}

void UMVVMLoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (int Index = 0; Index < NumLoadSlots; ++Index)
	{
		auto SaveObject = AuraGameMode->LoadSlotData(LoadSlots[Index]->GetLoadSlotName(), LoadSlots[Index]->SlotIndex);
		LoadSlots[Index]->SetPlayerName(SaveObject->PlayerName);
		LoadSlots[Index]->SlotStatus = SaveObject->SlotStatus;
		LoadSlots[Index]->InitializeSlot();
		LoadSlots[Index]->SetMapAssetName(SaveObject->MapName);
		LoadSlots[Index]->PlayerStartTag = SaveObject->PlayerStartTag;
		LoadSlots[Index]->SetPlayerLevel(SaveObject->PlayerLevel);
	}
}

void UMVVMLoadScreen::SetSelectedSlotIndex(const int32 NewSlotIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedSlotIndex, NewSlotIndex);
}
