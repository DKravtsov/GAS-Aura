// Copyright 4sandwiches


#include "Widgets/Utils/InventoryWidgetUtils.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"
#include "Items/InventoryItem.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverItemWidget.h"

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

FVector2D UInventoryWidgetUtils::GetWidgetSize(const UWidget* Widget)
{
	const FGeometry& Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

bool UInventoryWidgetUtils::IsWithinBounds(const FVector2D& WidgetPosition, const FVector2D& WidgetSize, const FVector2D& MousePosition)
{
	return MousePosition.X >= WidgetPosition.X && MousePosition.X <= (WidgetPosition.X + WidgetSize.X) &&
		MousePosition.Y >= WidgetPosition.Y && MousePosition.Y <= (WidgetPosition.Y + WidgetSize.Y);
}

const FInventoryItemImageFragment* UInventoryWidgetUtils::GetImageFragmentFromInventoryItem(const UInventoryItem* Item)
{
	return UInventoryItem::GetFragment<FInventoryItemImageFragment>(Item, InventoryFragmentTags::FragmentTag_Image);
}

const FInventoryItemGridFragment* UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(const UInventoryItem* Item)
{
	return UInventoryItem::GetFragment<FInventoryItemGridFragment>(Item, InventoryFragmentTags::FragmentTag_Grid);
}

FIntPoint UInventoryWidgetUtils::GetGridDimensionsOfItem(const UInventoryItem* Item)
{
	const auto GridFragment = GetGridFragmentFromInventoryItem(Item);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint{1,1};
	return Dimensions;
}

FVector2D UInventoryWidgetUtils::GetItemDrawSize(const UInventoryItem* Item, const float TileSize)
{
	const auto GridFragment = GetGridFragmentFromInventoryItem(Item);
	return GridFragment ? GetDrawSize(*GridFragment, TileSize) : FVector2D::ZeroVector;
}

FSlateBrush UInventoryWidgetUtils::GetTempBrush()
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = FVector2D{1,1};
	return Brush;
}

