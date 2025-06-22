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

UCLASS()
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
	int32 Rows = 0;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns = 0;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float TileSize = 0.f;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> GridWidget;

	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<UInventorySlottedItemWidget>> SlottedItems;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryHoverProxy> HoverItem;

	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;
	
public:

	FGameplayTag GetItemCategory() const { return ItemCategory; }

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
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
	void AssignHoverItem(UInventoryItem* ClickedItem, const int32 GridIndex = INDEX_NONE, const int32 PrevGridIndex = INDEX_NONE);
	void RemoveItemFromGrid(UInventoryItem* ClickedItem, const int32 GridIndex);
};
