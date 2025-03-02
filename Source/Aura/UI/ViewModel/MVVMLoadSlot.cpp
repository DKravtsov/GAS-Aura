// Copyright 4sandwiches


#include "UI/ViewModel/MVVMLoadSlot.h"

void UMVVMLoadSlot::InitializeSlot()
{
	const int32 WidgetIndex = static_cast<int32>(SlotStatus);
	OnSetWidgetSwitcherIndex.Broadcast(WidgetIndex);
}

void UMVVMLoadSlot::SetLoadSlotName(FString NewName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, NewName);
}

void UMVVMLoadSlot::SetPlayerName(FString NewName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, NewName);
}
