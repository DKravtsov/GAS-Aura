// Copyright 4sandwiches


#include "Widgets/HUD/InventoryInfoMessage.h"

#include "Components/TextBlock.h"

void UInventoryInfoMessage::SetMessage(const FText& Message)
{
	Text_Message->SetText(Message);

	if (!bMessageActive)
	{
		ShowMessage();
	}
	bMessageActive = true;

	GetWorld()->GetTimerManager().SetTimer(MessageTimer, [this]()
	{
		HideMessage();
		bMessageActive = false;
	}, MessageLifeTime, false);
}

void UInventoryInfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Text_Message->SetText(FText::GetEmpty());
	HideMessage();
}
