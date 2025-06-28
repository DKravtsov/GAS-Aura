// Copyright 4sandwiches


#include "Widgets/HUD/InventoryHUDWidget.h"

#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Widgets/HUD/InventoryInfoMessage.h"

void UInventoryHUDWidget::ShowPickupMessage(const FText& Message)
{
	BP_ShowPickupMessage(Message);
}

void UInventoryHUDWidget::HidePickupMessage()
{
	BP_HidePickupMessage();
}

void UInventoryHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UInventoryComponent* InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer()))
	{
		InventoryComponent->OnNoRoomInInventory.AddDynamic(this, &UInventoryHUDWidget::OnInventoryHasNoRoom);
	}
}

void UInventoryHUDWidget::OnInventoryHasNoRoom()
{
	if (IsValid(InfoMessage))
	{
		InfoMessage->SetMessage(NSLOCTEXT("Inventory", "InventoryHasNoRoom", "No Room In Inventory"));
	}
}
