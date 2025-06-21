// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridSlot.generated.h"

class UImage;

enum class EInventoryGridSlotVisualState : uint8
{
	Default,
	Occupied,
	Selected,
	GrayedOut,
};

UCLASS()
class UInventoryGridSlot : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	int32 TileIndex;
	int32 StackCount;
	int32 StartIndex = INDEX_NONE; // upper left index where the actual stack count is stored

	TWeakObjectPtr<class UInventoryItem> InventoryItem;

	EInventoryGridSlotVisualState GridSlotState;

	bool bIsAvailable = true;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush DefaultBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush OccupiedBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush SelectedBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush GrayedOutBrush;
	
public:

	int32 GetTileIndex() const { return TileIndex; }
	void SetTileIndex(const int32 NewTileIndex) { TileIndex = NewTileIndex; }

	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(const int32 NewStackCount) { StackCount = NewStackCount; }

	int32 GetStartIndex() const { return StartIndex; }
	void SetStartIndex(const int32 NewStartIndex) { StartIndex = NewStartIndex; }

	EInventoryGridSlotVisualState GetGridSlotState() const { return GridSlotState; }

	TWeakObjectPtr<UInventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UInventoryItem* Item);

	bool IsAvailable() const {return bIsAvailable;}
	void SetIsAvailable(bool bAvailable) { bIsAvailable = bAvailable; }
	
	void SetDefaultTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();

private:

	
};
