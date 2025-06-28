// Copyright 4sandwiches


#include "Widgets/Composite/InventoryLeafLabeledValue.h"

#include "Components/TextBlock.h"

void UInventoryLeafLabeledValue::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfo = Text_Label->GetFont();
	FontInfo.Size = LabelFontSize;
	Text_Label->SetFont(FontInfo);

	FontInfo = Text_Value->GetFont();
	FontInfo.Size = ValueFontSize;
	Text_Value->SetFont(FontInfo);

}

void UInventoryLeafLabeledValue::SetLabelText(const FText& NewText, bool bCollapse)
{
	Text_Label->SetText(NewText);
	if (bCollapse)
	{
		Text_Label->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryLeafLabeledValue::SetValueText(const FText& NewText, bool bCollapse)
{
	Text_Value->SetText(NewText);
	if (bCollapse)
	{
		Text_Value->SetVisibility(ESlateVisibility::Collapsed);
	}
}
