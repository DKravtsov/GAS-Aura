// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlottedItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClickedSignature, int32, GridIndex, const FPointerEvent&, MouseEvent);

UCLASS(MinimalAPI, Abstract)
class UInventorySlottedItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	FSlottedItemClickedSignature OnSlottedItemClicked;

private:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_StackCount;

	int32 GridIndex = INDEX_NONE;
	FIntPoint GridDimensions;
	TWeakObjectPtr<class UInventoryItem> InventoryItem;
	bool bIsStackable = false;

public:

	//~ Being of UUserWidget interface
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

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

	void UpdateStackCount(int32 NewStackCount);
};
