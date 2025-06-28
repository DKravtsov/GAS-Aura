// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridSlot.generated.h"

class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEventSignature, int32, GridIndex, const FPointerEvent&, MouseEvent);

enum class EInventoryGridSlotVisualState : uint8
{
	Default,
	Occupied,
	Selected,
	GrayedOut,
};

UCLASS(MinimalAPI, Abstract)
class UInventoryGridSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	FGridSlotEventSignature OnGridSlotClicked;
	FGridSlotEventSignature OnGridSlotHovered;
	FGridSlotEventSignature OnGridSlotUnhovered;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush DefaultBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush OccupiedBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush SelectedBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush GrayedOutBrush;

	int32 TileIndex = INDEX_NONE;
	int32 StackCount = 0;
	int32 StartIndex = INDEX_NONE; // upper left index where the actual stack count is stored

	TWeakObjectPtr<class UInventoryItem> InventoryItem;

	EInventoryGridSlotVisualState GridSlotState;

	bool bIsAvailable = true;
	
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
	void SetGridSlotState(EInventoryGridSlotVisualState NewState);

	//~ Begin of UUserWidget interface
	virtual void NativePreConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

private:

	
};
