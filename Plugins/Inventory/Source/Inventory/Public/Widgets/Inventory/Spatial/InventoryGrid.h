// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridTypes.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "InventoryGrid.generated.h"


class UInventoryGridSlot;
class UUniformGridPanel;
class UGridPanel;
enum class EInventoryGridSlotVisualState : uint8;

UCLASS(MinimalAPI, Abstract)
class UInventoryGrid : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta=(Categories="Inventory.ItemCategory", AllowPrivateAccess=true))
	FGameplayTag ItemCategory;

	UPROPERTY()
	TArray<TObjectPtr<UInventoryGridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInventoryGridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventorySlottedItemWidget> SlottedItemClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryHoverProxy> HoverItemClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryItemPopup> ItemPopupMenuClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Rows = 0;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns = 0;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float TileSize = 0.f;

	// A small offset (in screen coordinates) for the popup menu related to the mouse cursor position
	UPROPERTY(EditAnywhere, Category="Inventory")
	FVector2D PopupMenuOffset{5.f, 5.f};

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> GridWidget;

	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<UInventorySlottedItemWidget>> SlottedItems;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryHoverProxy> HoverItem;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryItemPopup> ItemPopupMenu;

	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<class UCanvasPanel> OwningCanvasPanel;

	FInventoryTileParameters TileParameters;
	FInventoryTileParameters LastTileParameters;

	// Index where an item would be placed if we click on the grid at a valid location. It's Valid only when there's a hover item
	int32 ItemDropIndex = INDEX_NONE;
	// More detailed information about where a hovered item would be placed. It's Valid only when there's a hover item
	FInventorySpaceQueryResult CurrentQueryResult;

	int32 LastHighlightedIndex = INDEX_NONE;
	FIntPoint LastHighlightedDimensions;

	uint8 bMouseWithinCanvas:1 = false;
	uint8 bMouseWasWithinCanvas:1 = false;
	
public:

	FGameplayTag GetItemCategory() const { return ItemCategory; }

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
	INVENTORY_API virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	//~ End of UUserWidget interface

	int32 GetIndexFromPosition(const FIntPoint& Position) const
	{
		return Position.X + Position.Y * Columns;
	}

	FIntPoint GetPositionFromIndex(int32 Index) const
	{
		return FIntPoint(Index % Columns, Index / Columns);
	}

	UFUNCTION()
	INVENTORY_API void AddItem(UInventoryItem* Item);

	UFUNCTION()
	INVENTORY_API void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	INVENTORY_API FInventorySlotAvailabilityResult HasRoomForItem(const class UInventoryItemComponent* ItemComponent) const;

	//~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface

	void SetOwningCanvas(UCanvasPanel* OwningCanvas);

private:

	void ConstructGrid();
	bool MatchesCategory(const UInventoryItem* Item) const;

	UFUNCTION()
	void OnStackChanged(const FInventorySlotAvailabilityResult& Result);

	FInventorySlotAvailabilityResult HasRoomForItemInternal(const struct FInventoryItemManifest& ItemManifest) const;

	void AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem);
	UInventorySlottedItemWidget* CreateSlottedItemWidget(UInventoryItem* Item, int32 Index,
	                                                     const FInventoryItemGridFragment& GridFragment,
	                                                     const FInventoryItemImageFragment& ImageFragment, bool bStackable, int32 StackAmount);
	void AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);

	FVector2D GetDrawSize(const struct FInventoryItemGridFragment& GridFragment) const;
	void SetSlottedItemImage(const UInventorySlottedItemWidget* SlottedItem, const FInventoryItemGridFragment& GridFragment,
	                         const FInventoryItemImageFragment& ImageFragment) const;

	void AddSlottedItemToGrid(const int32 Index, const FInventoryItemGridFragment& GridFragment, UInventorySlottedItemWidget* SlottedItem) const;
	
	static const FInventoryItemImageFragment* GetImageFragmentFromInventoryItem(UInventoryItem* Item);
	static const FInventoryItemGridFragment* GetGridFragmentFromInventoryItem(UInventoryItem* NewItem);

	void UpdateGridSlots(UInventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount);

	bool HasRoomAtIndex(const UInventoryGridSlot* GridSlot, const FIntPoint& Dimensions,
						const TSet<int32>& CheckedIndexes, TSet<int32>& OutTentativelyClaimedIndexes,
						const int32 MaxStackSize, const FGameplayTag& ItemType) const;

	bool CheckSlotConstraints(const UInventoryGridSlot* GridSlot, const UInventoryGridSlot* CurGridSlot,
							  const TSet<int32>& CheckedIndexes, TSet<int32>& OutTentativelyClaimedIndexes,
							  const int32 MaxStackSize, const FGameplayTag& ItemType) const;

	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& Dimensions) const;

	int32 GetStackAmountInSlot(const UInventoryGridSlot* GridSlot) const;
	int32 CalculateStackableFillAmountForSlot(const int32 MaxStackSize, const int32 Amount, const UInventoryGridSlot* GridSlot) const;

	static bool IsLeftMouseButtonClick(const FPointerEvent& MouseEvent);
	static bool IsRightMouseButtonClick(const FPointerEvent& MouseEvent);

	void PickUpItemInInventory(UInventoryItem* ClickedItem, const int32 GridIndex);
	void PutDownItemInInventoryAtIndex(const int32 GridIndex);
	void ShowDefaultCursor() const;
	void AssignHoverItem(UInventoryItem* ClickedItem, const int32 GridIndex = INDEX_NONE, const int32 PrevGridIndex = INDEX_NONE);
	void RemoveItemFromGrid(UInventoryItem* ClickedItem, const int32 GridIndex);

	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);
	void OnTileParametersUpdated(const FInventoryTileParameters& Parameters);
	bool CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location);
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	EInventoryTileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	FIntPoint CalculateStartingCoordinates(const FIntPoint& Coordinate, const FIntPoint& Dimensions, EInventoryTileQuadrant Quadrant) const;

	FInventorySpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions) const;
	void ClearHoverItem();
	void SwapWithHoverItem(UInventoryItem* ClickedInventoryItem, const int32 GridIndex);
	void SwapStackCountsWithHoverItem(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 GridIndex);
	void FillInStacksOrConsumeHover(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 MaxStackCount, const int32 GridIndex);
	
	void UpdateStackCountInSlot(int32 GridIndex, int32 NewStackCount);

	void HighlightSlots(const int32 StartIndex, const FIntPoint& Dimensions);
	void UnHighlightSlots(const int32 StartIndex, const FIntPoint& Dimensions);
	void UnHighlightSlots() {UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);}
	void ChangeHoverType(const int32 StartIndex, const FIntPoint& Dimensions, EInventoryGridSlotVisualState GridSlotState);

	UFUNCTION()
	void OnGridSlotClicked(int32 GridSlotIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotHovered(int32 GridSlotIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridSlotIndex, const FPointerEvent& MouseEvent);

	bool IsHoverItemSameStackableAs(UInventoryItem* ClickedInventoryItem) const;

	static int32 GetMaxStackSize(const UInventoryItem* Item);

	void CreateItemPopupMenu(const int32 GridIndex);

	void OnPopupMenuSplit(const int32 SplitAmount, const int32 GridIndex);
	void OnPopupMenuConsume(const int32 GridIndex);
	void OnPopupMenuDrop(const int32 GridIndex);
};
