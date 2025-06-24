// Copyright 4sandwiches


#include "InventoryItemPopup.h"

#include "Components/Button.h"
#include "Components/Slider.h"

void UInventoryItemPopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Split->OnClicked.AddDynamic(this, &UInventoryItemPopup::SplitButtonClicked);
	Button_Consume->OnClicked.AddDynamic(this, &UInventoryItemPopup::ConsumeButtonClicked);
	Button_Drop->OnClicked.AddDynamic(this, &UInventoryItemPopup::DropButtonClicked);
	Slider_Split->OnValueChanged.AddDynamic(this, &UInventoryItemPopup::SliderValueChanged);
}

void UInventoryItemPopup::SliderValueChanged(float Value)
{
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

int32 UInventoryItemPopup::GetSplitAmount() const
{
	return FMath::FloorToInt32(Slider_Split->GetValue());
}

void UInventoryItemPopup::HideMenu()
{
	RemoveFromParent();
}
