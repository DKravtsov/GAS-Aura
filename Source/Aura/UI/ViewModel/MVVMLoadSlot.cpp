// Copyright 4sandwiches


#include "UI/ViewModel/MVVMLoadSlot.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

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

FText UMVVMLoadSlot::GetMapName() const
{
	AAuraGameModeBase* AuraGameMode = CastChecked<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	return AuraGameMode->GetMapDisplayName(BoundMap);
}

void UMVVMLoadSlot::SetMap(const TSoftObjectPtr<UWorld>& NewMap)
{
	if (BoundMap != NewMap)
	{
		BoundMap = NewMap;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetMapName);
	}
}
