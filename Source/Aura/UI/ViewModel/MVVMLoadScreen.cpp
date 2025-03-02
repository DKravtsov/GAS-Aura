// Copyright 4sandwiches


#include "UI/ViewModel/MVVMLoadScreen.h"

#include "MVVMLoadSlot.h"

void UMVVMLoadScreen::InitializeLoadSlots()
{
	for (int32 Index = 0; Index < NumLoadSlots; ++Index)
	{
		LoadSlots[Index] = NewObject<UMVVMLoadSlot>(this, LoadSlotViewModelClass);
		LoadSlots[Index]->SetLoadSlotName(FString::Printf(TEXT("LoadSlot_%d"), Index));
	}
}

UMVVMLoadSlot* UMVVMLoadScreen::GetLoadSlot(int32 Index) const
{
	checkf(Index >= 0 && Index < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);
	return LoadSlots[Index];
}

void UMVVMLoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	checkf(Slot >= 0 && Slot < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);
}

void UMVVMLoadScreen::NewGameButtonPressed(int32 Slot)
{
	checkf(Slot >= 0 && Slot < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);
	LoadSlots[Slot]->OnSetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVMLoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	checkf(Slot >= 0 && Slot < NumLoadSlots, TEXT("Only %d Load slots available in this version"), NumLoadSlots);
}
