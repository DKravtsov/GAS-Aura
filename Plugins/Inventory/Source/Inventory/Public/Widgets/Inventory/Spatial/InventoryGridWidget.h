// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridTypes.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "InventoryGridWidget.generated.h"


class UInventoryStoreComponent;
struct FInventoryItemManifest;
class UInventoryGridSlotWidget;
class UUniformGridPanel;
class UGridPanel;
enum class EInventoryGridSlotVisualState : uint8;

UCLASS(MinimalAPI, Abstract)
class UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta=(Categories="Inventory.ItemCategory", AllowPrivateAccess=true))
	FGameplayTag ItemCategory;

	UPROPERTY()
	TArray<TObjectPtr<UInventoryGridSlotWidget>> GridSlots;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInventoryGridSlotWidget> GridSlotClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventorySlottedItemWidget> SlottedItemClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryHoverItemWidget> HoverItemClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UInventoryItemPopup> ItemPopupMenuClass;

	int32 Rows = 0;
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
	TObjectPtr<UInventoryHoverItemWidget> HoverItem;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryItemPopup> ItemPopupMenu;

	UPROPERTY(Transient)
	TObjectPtr<class UInventoryGridViewModel> GridViewModel;

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

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(AllowPrivateAccess=true))
	uint8 bIsStoreGrid:1 = false;
	
public:

	FGameplayTag GetItemCategory() const { return ItemCategory; }

	void SetIsStoreGrid(bool bStoreGrid) { bIsStoreGrid = bStoreGrid; }
	bool IsStoreGrid() const { return bIsStoreGrid; }

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
	INVENTORY_API virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	//~ End of UUserWidget interface

	void ConstructForStore(UInventoryStoreComponent* Store);

	float GetTileSize() const { return TileSize; }

	UFUNCTION()
	INVENTORY_API void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	//~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface

	void SetOwningCanvas(UCanvasPanel* OwningCanvas);

	void DropHoverItemOnGround();

	bool HasHoverItem() const;
	UInventoryHoverItemWidget* GetHoverItem() const {return HoverItem;}
	void ClearHoverItem();

	void OnHide();

	void HandleAddItemToGrid(const FInventorySlotAvailabilityResult& Result);
	void HandleOnStackChanged(const FInventorySlotAvailabilityResult& Result);
	void HandleOnUpdateGridSlots(const TArrayView<int32>& GridIndexArray);
	void HandleOnRemovedItemFromGrid(const TArrayView<int32>& GridIndexArray);

	void HandleOnHoverItemReset();
	void HandleOnHoverItemUpdated(UInventoryItem* Item, bool bStackable, int32 StackCount, int32 PreviousIndex);

	int32 GetNumGridSlots() const {return GridSlots.Num();}

	void UpdateInventoryGridSlots();

private:

	void ConstructGrid();
	void CreateGridViewModel(class UInventoryStorage* InventoryStorage);
	
	bool MatchesCategory(const UInventoryItem* Item) const;

	void AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem);
	UInventorySlottedItemWidget* CreateSlottedItemWidget(UInventoryItem* Item, int32 Index,
	                                                     const FInventoryItemGridFragment& GridFragment,
	                                                     const FInventoryItemImageFragment& ImageFragment, bool bStackable, int32 StackAmount);
	void AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);

	void SetSlottedItemImage(const UInventorySlottedItemWidget* SlottedItem, const FInventoryItemGridFragment& GridFragment,
	                         const FInventoryItemImageFragment& ImageFragment) const;

	void AddSlottedItemToGrid(const int32 Index, const FInventoryItemGridFragment& GridFragment, UInventorySlottedItemWidget* SlottedItem) const;

	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& Dimensions) const;

	static bool IsLeftMouseButtonClick(const FPointerEvent& MouseEvent);
	static bool IsRightMouseButtonClick(const FPointerEvent& MouseEvent);

	void PickUpItemInInventory(UInventoryItem* ClickedItem, const int32 GridIndex);
	void PutDownItemInInventoryAtIndex(const int32 GridIndex);
	void ShowDefaultCursor() const;

	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);
	void OnTileParametersUpdated(const FInventoryTileParameters& Parameters);
	bool CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location);
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	EInventoryTileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	FIntPoint CalculateStartingCoordinates(const FIntPoint& Coordinate, const FIntPoint& Dimensions, EInventoryTileQuadrant Quadrant) const;

	FInventorySpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions) const;
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

	bool IsHoverItemSameStackableAs(const UInventoryItem* ClickedInventoryItem) const;

	static int32 GetMaxStackSize(const UInventoryItem* Item);

	void CreateItemPopupMenu(const int32 GridIndex);

	void OnPopupMenuSplit(const int32 SplitAmount, const int32 GridIndex);
	void OnPopupMenuConsume(const int32 GridIndex);
	void OnPopupMenuDrop(const int32 GridIndex);
	void OnPopupMenuBuy(const int32 GridIndex);
	void OnPopupMenuSell(const int32 GridIndex);

	static FSlateBrush GetTempBrush();

	void PutHoverItemDown();

	UInventorySlottedItemWidget* GetSlottedItemWidgetAtIndex(const int32 UpperLeftIndex) const;
};
