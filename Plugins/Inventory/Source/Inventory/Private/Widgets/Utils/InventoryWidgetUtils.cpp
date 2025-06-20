// Copyright 4sandwiches


#include "Widgets/Utils/InventoryWidgetUtils.h"

int32 UInventoryWidgetUtils::GetIndexFromPosition(const FIntPoint& Position, int32 Columns)
{
	return Position.X + Position.Y * Columns;
}
