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
	Button_Sell->OnClicked.AddDynamic(this, &UInventoryItemPopup::SellButtonClicked);
	Button_Buy->OnClicked.AddDynamic(this, &UInventoryItemPopup::BuyButtonClicked);

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
	Slider_Split->SetMaxValue(StackCount - 1);
	Slider_Split->SetMinValue(1);
	Slider_Split->SetValue(FMath::Max(StackCount / 2, 1));
}

void UInventoryItemPopup::SliderValueChanged(float Value)
{
	Text_SliderAmount->SetText(FText::AsNumber(FMath::Floor(Value)));
}

void UInventoryItemPopup::SplitButtonClicked()
{
	OnSplitDelegate.ExecuteIfBound(GetSplitAmount(), GridIndex);
	HideMenu();
}

void UInventoryItemPopup::DropButtonClicked()
{
	OnDropDelegate.ExecuteIfBound(GridIndex);
	HideMenu();
}

void UInventoryItemPopup::ConsumeButtonClicked()
{
	OnConsumeDelegate.ExecuteIfBound(GridIndex);
	HideMenu();
}

void UInventoryItemPopup::SellButtonClicked()
{
	OnSellItemDelegate.ExecuteIfBound(GridIndex);
	HideMenu();
}

void UInventoryItemPopup::BuyButtonClicked()
{
	OnBuyItemDelegate.ExecuteIfBound(GridIndex);
	HideMenu();
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

void UInventoryItemPopup::CollapseSellButton()
{
	Button_Sell->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryItemPopup::CollapseBuyButton()
{
	Button_Buy->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryItemPopup::CollapseDropButton()
{
	Button_Drop->SetVisibility(ESlateVisibility::Collapsed);
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
	GridIndex = INDEX_NONE;
}
