// Copyright 4sandwiches


#include "Widgets/Inventory/ItemPopUp/InventoryItemPopup.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UInventoryItemPopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Split->OnClicked.AddDynamic(this, &UInventoryItemPopup::SplitButtonClicked);
	Button_Consume->OnClicked.AddDynamic(this, &UInventoryItemPopup::ConsumeButtonClicked);
	Button_Drop->OnClicked.AddDynamic(this, &UInventoryItemPopup::DropButtonClicked);
	Slider_Split->OnValueChanged.AddDynamic(this, &UInventoryItemPopup::SliderValueChanged);

	Slider_Split->SetStepSize(1);
	Slider_Split->MouseUsesStep = true;
}

void UInventoryItemPopup::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	HideMenu();
}

void UInventoryItemPopup::SetSliderParams(const int32 MaxStackSize, const int32 StackCount)
{
	Slider_Split->SetMaxValue(MaxStackSize);
	Slider_Split->SetMinValue(1);
	Slider_Split->SetValue(StackCount);
}

void UInventoryItemPopup::SliderValueChanged(float Value)
{
	Text_SliderAmount->SetText(FText::AsNumber(FMath::Floor(Value)));
}

void UInventoryItemPopup::SplitButtonClicked()
{
	if (OnSplitDelegate.ExecuteIfBound(GetSplitAmount(), GridIndex))
	{
		HideMenu();
	}
}

void UInventoryItemPopup::DropButtonClicked()
{
	if (OnDropDelegate.ExecuteIfBound(GridIndex))
	{
		HideMenu();
	}
}

void UInventoryItemPopup::ConsumeButtonClicked()
{
	if (OnConsumeDelegate.ExecuteIfBound(GridIndex))
	{
		HideMenu();
	}
}

void UInventoryItemPopup::CollapseSplitButton()
{
	Button_Split->SetVisibility(ESlateVisibility::Collapsed);
	Slider_Split->SetVisibility(ESlateVisibility::Collapsed);
	Text_SliderAmount->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryItemPopup::CollapseConsumeButton()
{
	Button_Consume->SetVisibility(ESlateVisibility::Collapsed);
}

FVector2D UInventoryItemPopup::GetBoxSize() const
{
	return FVector2D(SizeBox_Root->GetWidthOverride(), SizeBox_Root->GetHeightOverride());
}

int32 UInventoryItemPopup::GetSplitAmount() const
{
	return FMath::FloorToInt32(Slider_Split->GetValue());
}

void UInventoryItemPopup::HideMenu()
{
	RemoveFromParent();
}
