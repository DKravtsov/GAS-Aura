// Copyright 4sandwiches


#include "Widgets/Composite/InventoryLeafImage.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"

void UInventoryLeafImage::SetImage(UTexture2D* Texture)
{
	Image_Icon->SetBrushFromTexture(Texture);
}

void UInventoryLeafImage::SetImageFromSoftTexture(const TSoftObjectPtr<UTexture2D>& Texture)
{
	Image_Icon->SetBrushFromSoftTexture(Texture);
}

void UInventoryLeafImage::SetBoxSize(const FVector2D& Size)
{
	SizeBox_Icon->SetWidthOverride(Size.X);
	SizeBox_Icon->SetHeightOverride(Size.Y);
}

void UInventoryLeafImage::SetImageSize(const FVector2D& Size)
{
	Image_Icon->SetDesiredSizeOverride(Size);
}

FVector2D UInventoryLeafImage::GetImageSize() const
{
	return Image_Icon->GetDesiredSize();
}
