// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVMLoadSlot.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndexSignature, int32, WidgetSwitcherIndex);

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

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess="true"))
	FString LoadSlotName;
public:

	void InitializeSlot();

	FString GetLoadSlotName() const { return LoadSlotName; }
	void SetLoadSlotName(FString NewName);
};
