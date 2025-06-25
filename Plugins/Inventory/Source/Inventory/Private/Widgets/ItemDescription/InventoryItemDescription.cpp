// Copyright 4sandwiches


#include "Widgets/ItemDescription/InventoryItemDescription.h"

#include "Components/SizeBox.h"

FVector2D UInventoryItemDescription::GetBoxSize() const
{
	return SizeBox_Root->GetDesiredSize();
}
