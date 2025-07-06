// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHoverProxyWidget.generated.h"

struct FGameplayTag;
class UImage;
class UTextBlock;

/**
 * The HoverProxy is the item that will appear and follow the mouse
 * when an inventory item on the grid has been clicked.
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryHoverProxyWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 PreviousGridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<class UInventoryItem> InventoryItem;
	int32 StackCount = 0;
	bool bIsStackable = false;

public:

	void SetImageBrush(const FSlateBrush& Brush);

	void UpdateStackCount(const int32 NewStackCount);

	const FGameplayTag& GetItemType() const;
	
	int32 GetStackCount() const {return StackCount;}
	
	bool IsStackable() const {return bIsStackable;}
	void SetIsStackable(bool bStackable);

	int32 GetPreviousGridIndex() const {return PreviousGridIndex;}
	void SetPreviousGridIndex(int32 NewPrevGridIndex) {PreviousGridIndex = NewPrevGridIndex;}

	const FIntPoint& GetGridDimensions() const {return GridDimensions;}
	void SetGridDimensions(const FIntPoint& NewGridDimensions) {GridDimensions = NewGridDimensions;}

	UInventoryItem* GetInventoryItem() const {return InventoryItem.Get();}
	void SetInventoryItem(UInventoryItem* Item);

	void Reset();
};
