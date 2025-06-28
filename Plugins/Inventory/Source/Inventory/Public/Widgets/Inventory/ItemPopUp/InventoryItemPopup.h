// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemPopup.generated.h"

class UButton;

DECLARE_DELEGATE_TwoParams(FInventoryItemPopupMenuSplitDelegate, const int32 /*SplitAmount*/, const int32 /*GridIndex*/);
DECLARE_DELEGATE_OneParam(FInventoryItemPopupMenuDropDelegate, const int32 /*GridIndex*/);
DECLARE_DELEGATE_OneParam(FInventoryItemPopupMenuConsumeDelegate, const int32 /*GridIndex*/);

/**
 * The item popup widget when a user clicks RMB on the item
 */
UCLASS(Abstract)
class UInventoryItemPopup : public UUserWidget
{
	GENERATED_BODY()

public:

	FInventoryItemPopupMenuSplitDelegate OnSplitDelegate;
	FInventoryItemPopupMenuDropDelegate OnDropDelegate;
	FInventoryItemPopupMenuConsumeDelegate OnConsumeDelegate;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Split;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> Slider_Split;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_SliderAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Root;

	int32 GridIndex = INDEX_NONE;

public:

	//~ Begin of UUserWidget interface
	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

	void SetSliderParams(const int32 MaxStackSize, const int32 StackCount);
	int32 GetSplitAmount() const;
	
	void HideMenu();

	int32 GetGridIndex() const {return GridIndex;}
	void SetGridIndex(int32 NewValue) {GridIndex = NewValue;}

	void CollapseSplitButton();
	void CollapseConsumeButton();

	FVector2D GetBoxSize() const;

private:

	UFUNCTION()
	void SliderValueChanged(float Value);
	
	UFUNCTION()
	void SplitButtonClicked();

	UFUNCTION()
	void DropButtonClicked();

	UFUNCTION()
	void ConsumeButtonClicked();
};
