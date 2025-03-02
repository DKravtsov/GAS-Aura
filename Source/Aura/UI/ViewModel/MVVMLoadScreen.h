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

	static constexpr int32 NumLoadSlots = 3;
	
	UPROPERTY()
	TObjectPtr<UMVVMLoadSlot> LoadSlots[NumLoadSlots];

public:

	void InitializeLoadSlots();

	UFUNCTION(BlueprintPure)
	UMVVMLoadSlot* GetLoadSlot(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);
};
