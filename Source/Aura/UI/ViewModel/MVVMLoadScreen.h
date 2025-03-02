// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVMLoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelectedSignature);

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

	UPROPERTY(BlueprintAssignable)
	FSlotSelectedSignature OnSlotSelected;

	UPROPERTY(BlueprintAssignable)
	FSlotSelectedSignature OnSlotSelectionCleared;
	
private:

	static constexpr int32 NumLoadSlots = 3;
	
	UPROPERTY()
	TObjectPtr<UMVVMLoadSlot> LoadSlots[NumLoadSlots];

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess="true"))
	int32 SelectedSlotIndex = INDEX_NONE;

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

	UFUNCTION(BlueprintCallable)
	void PlayFromSelectedSlot();

	UFUNCTION(BlueprintCallable)
	void DeleteSelectedSlot();

	void LoadData();

	int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }
	void SetSelectedSlotIndex(const int32 NewSlotIndex);
};
