// Copyright 4sandwiches


#include "Widgets/Utils/InventoryWidgetUtils.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"

int32 UInventoryWidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}

FIntPoint UInventoryWidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}

FVector2D UInventoryWidgetUtils::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry& Geometry = Widget->GetCachedGeometry();

	const FVector2D LocalTopLeft = USlateBlueprintLibrary::GetLocalTopLeft(Geometry);
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry, LocalTopLeft, PixelPosition, ViewportPosition);

	return ViewportPosition;
}
