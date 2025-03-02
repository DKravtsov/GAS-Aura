// Copyright 4sandwiches


#include "UI/ViewModel/MVVMLoadSlot.h"

void UMVVMLoadSlot::InitializeSlot()
{
	//OnSetWidgetSwitcherIndex.Broadcast(?);
}

void UMVVMLoadSlot::SetLoadSlotName(FString NewName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, NewName);
}
