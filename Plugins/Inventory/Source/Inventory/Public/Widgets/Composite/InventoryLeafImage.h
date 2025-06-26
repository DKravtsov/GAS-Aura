// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryLeaf.h"
#include "InventoryLeafImage.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryLeafImage : public UInventoryLeaf
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Icon;
	
public:

	void SetImage(UTexture2D* Texture);
	void SetImageFromSoftTexture(const TSoftObjectPtr<UTexture2D>& Texture);
	void SetBoxSize(const FVector2D& Size);
	void SetImageSize(const FVector2D& Size);
	FVector2D GetImageSize() const;
};
