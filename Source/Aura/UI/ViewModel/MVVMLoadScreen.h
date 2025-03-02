// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVMLoadScreen.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMVVMLoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMVVMLoadSlot> LoadSlotViewModelClass;

private:

	UPROPERTY()
	TArray<TObjectPtr<UMVVMLoadSlot>> LoadSlots;

public:

	void InitializeLoadSlots();

	UFUNCTION(BlueprintPure)
	UMVVMLoadSlot* GetLoadSlot(int32 Index) const;
};
