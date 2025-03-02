// Copyright 4sandwiches


#include "UI/ViewModel/MVVMLoadScreen.h"

#include "MVVMLoadSlot.h"

void UMVVMLoadScreen::InitializeLoadSlots()
{
	LoadSlots.SetNum(3);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		LoadSlots[Index] = NewObject<UMVVMLoadSlot>(this, LoadSlotViewModelClass);
	}
}

UMVVMLoadSlot* UMVVMLoadScreen::GetLoadSlot(int32 Index) const
{
	checkf(Index >= 0 && Index < 3, TEXT("Only 3 Load slots available in this version"));
	return LoadSlots[Index];
}
