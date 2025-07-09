// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridSlotWidget.generated.h"

class UInventoryItem;
struct FInventoryStorageGridSlot;
class UInventoryStorageGrid;
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
class UInventoryGridSlotWidget : public UUserWidget
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

	EInventoryGridSlotVisualState GridSlotState;

	int32 GridIndex = INDEX_NONE;

	TWeakObjectPtr<UInventoryStorageGrid> StorageGrid;

	bool bIsAvailable = true;
	
public:

	void Bind(UInventoryStorageGrid* InStorageGrid, int32 InTileIndex);
	
	const FInventoryStorageGridSlot& GetStorageSlot() const;

	int32 GetTileIndex() const {return GridIndex;}

	int32 GetStackCount() const;
	//void SetStackCount(const int32 NewStackCount);

	int32 GetStartIndex() const;
	//void SetStartIndex(const int32 NewStartIndex) { StartIndex = NewStartIndex; }

	EInventoryGridSlotVisualState GetGridSlotState() const { return GridSlotState; }

	TWeakObjectPtr<UInventoryItem> GetInventoryItem() const;
	//void SetInventoryItem(UInventoryItem* Item);

	bool IsAvailable() const;// {return bIsAvailable;}
	void SetIsAvailable(bool bAvailable) { check(GridIndex == INDEX_NONE); bIsAvailable = bAvailable; }

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

	FInventoryStorageGridSlot& GetStorageSlotMutable();
	
};
