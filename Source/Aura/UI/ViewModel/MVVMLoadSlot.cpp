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
	return AuraGameMode->GetMapDisplayNameByAssetName(MapAssetName);
}

void UMVVMLoadSlot::SetMap(const TSoftObjectPtr<UWorld>& NewMap)
{
	if (MapAssetName != NewMap.ToSoftObjectPath().GetAssetName())
	{
		MapAssetName = NewMap.ToSoftObjectPath().GetAssetName();
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetMapName);
	}
}

void UMVVMLoadSlot::SetMapAssetName(const FString& NewMapAssetName)
{
	MapAssetName = NewMapAssetName;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetMapName);
}

void UMVVMLoadSlot::SetPlayerLevel(const int32 NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, NewValue);
}
