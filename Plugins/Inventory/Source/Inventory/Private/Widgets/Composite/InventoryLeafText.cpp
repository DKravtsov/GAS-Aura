// Copyright 4sandwiches


#include "Widgets/Composite/InventoryLeafText.h"

#include "Components/TextBlock.h"

void UInventoryLeafText::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfo = Text_Body->GetFont();
	FontInfo.Size = FontSize;
	Text_Body->SetFont(FontInfo);
}

void UInventoryLeafText::SetText(const FText& NewText)
{
	Text_Body->SetText(NewText);
}
