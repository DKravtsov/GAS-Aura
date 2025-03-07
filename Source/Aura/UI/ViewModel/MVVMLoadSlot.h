// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVMLoadSlot.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndexSignature, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetEnableSlotSignature, bool, bEnable);

/**
 * 
 */
UCLASS()
class AURA_API UMVVMLoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndexSignature OnSetWidgetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FSetEnableSlotSignature OnSetEnableSlot;

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	ESaveSlotStatus SlotStatus = ESaveSlotStatus::Vacant;

	UPROPERTY()
	FName PlayerStartTag;

private:

	UPROPERTY()
	FString MapAssetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess="true"))
	FString LoadSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess="true"))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess="true"))
	int32 PlayerLevel;

public:

	void InitializeSlot();

	FString GetLoadSlotName() const { return LoadSlotName; }
	void SetLoadSlotName(FString NewName);

	FString GetPlayerName() const { return PlayerName; }
	void SetPlayerName(FString NewName);

	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetMapName() const;
	
	void SetMap(const TSoftObjectPtr<UWorld>& NewMap);
	const FString& GetMapAssetName() const { return MapAssetName; }
	void SetMapAssetName(const FString& NewMapAssetName);

	int32 GetPlayerLevel() const { return PlayerLevel; }
	void SetPlayerLevel(const int32 NewValue);
};
