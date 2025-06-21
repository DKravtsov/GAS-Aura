// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlottedItemWidget.generated.h"

UCLASS()
class UInventorySlottedItemWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Icon;

	int32 GridIndex = INDEX_NONE;
	FIntPoint GridDimensions;
	TWeakObjectPtr<class UInventoryItem> InventoryItem;
	bool bIsStackable = false;

public:

	void SetIsStackable(bool bStackable) {bIsStackable = bStackable;}
	bool IsStackable() const {return bIsStackable;}

	UImage* GetImageIcon() const {return Image_Icon;}

	void SetGridIndex(int32 NewGridIndex) {GridIndex = NewGridIndex;}
	int32 GetGridIndex() const {return GridIndex;}

	void SetGridDimensions(FIntPoint NewGridDimensions) {GridDimensions = NewGridDimensions;}
	const FIntPoint& GetGridDimensions() const {return GridDimensions;}

	void SetInventoryItem(UInventoryItem* Item);
	UInventoryItem* GetInventoryItem() const {return InventoryItem.Get();}

	void SetImageBrush(const FSlateBrush& Brush) const;
};
