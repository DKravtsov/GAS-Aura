// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "UObject/Object.h"
#include "InventoryStorageGrid.generated.h"

struct FInventoryItemManifest;
/**
 * 
 */
UCLASS(MinimalAPI)
class UInventoryStorageGrid : public UObject
{
	GENERATED_BODY()

public:

	FInventoryItemGridChangedDelegate OnItemAdded;
	FInventoryItemChangedDelegate OnItemRemoved;
	FInventoryItemGridChangedDelegate OnStackChanged;
	FInventoryGridSlotsUpdatedSignature OnGridSlotsUpdated;
	FInventoryGridSlotsUpdatedSignature OnGridSlotsReset;

private:
	
	//~ todo: Replace with FastArray
	UPROPERTY(Replicated)
	TArray<FInventoryStorageGridSlot> GridSlots;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Rows = 1;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns = 1;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FGameplayTag ItemCategory;

	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<AActor> OwningActor;

public:
	int32 GetIndexFromPosition(const FIntPoint& Position) const
	{
		return Position.X + Position.Y * Columns;
	}

	FIntPoint GetPositionFromIndex(int32 Index) const
	{
		return FIntPoint(Index % Columns, Index / Columns);
	}

	const FGameplayTag& GetItemCategory() const {return ItemCategory;}
	void SetItemCategory(const FGameplayTag& GameplayTag) {ItemCategory = GameplayTag;}

	int32 GetRows() const {return Rows;}
	int32 GetColumns() const {return Columns;}
	const TArray<FInventoryStorageGridSlot>& GetGridSlots() const {return GridSlots;}
	TArray<FInventoryStorageGridSlot>& GetGridSlotsMutable() {return GridSlots;}
	FInventoryStorageGridSlot& GetGridSlotMutable(int32 Index);
	const FInventoryStorageGridSlot& GetGridSlot(int32 Index) const;
	FInventoryStorageGridSlot& GetGridSlotMutable(const FIntPoint& Position);
	const FInventoryStorageGridSlot& GetGridSlot(const FIntPoint& Position) const;

	FInventorySlotAvailabilityResult HasRoomForItem(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride = -1) const;
	
	void ConstructGrid(int32 InNumRows, int32 InNumColumns);

	//~ Begin of UObject interface
	virtual bool IsSupportedForNetworking() const override {return true;}
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End of UObject interface
	
	UFUNCTION()
	void HandleStackChanged(const FInventorySlotAvailabilityResult& Result);
	
	void UpdateGridSlots(UInventoryItem* NewItem, int32 Index, bool bStackable, int32 StackAmount);
	void RemoveItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex);

private:
	void ConstructGrid();
	
	bool IsInGridBounds(int32 StartIndex, const FIntPoint& Dimensions) const;

	bool HasRoomAtIndex(const FInventoryStorageGridSlot& GridSlot, const FIntPoint& Dimensions,
							const TSet<int32>& CheckedIndexes, TSet<int32>& OutTentativelyClaimedIndexes,
							const int32 MaxStackSize, const FGameplayTag& ItemType) const;


	static bool CheckSlotConstraints(const FInventoryStorageGridSlot& GridSlot,
									 const FInventoryStorageGridSlot& CurGridSlot,
									 const TSet<int32>& CheckedIndexes,
									 TSet<int32>& OutTentativelyClaimedIndexes,
									 const int32 MaxStackSize,
									 const FGameplayTag& ItemType);
	int32 CalculateStackableFillAmountForSlot(int32 MaxStackSize, int32 Amount,
											  const FInventoryStorageGridSlot& GridSlot) const;
	int32 GetStackAmountInSlot(const FInventoryStorageGridSlot& GridSlot) const;

	UFUNCTION()
	void HandleItemAdded(UInventoryItem* Item);

	bool MatchesCategory(UInventoryItem* Item);

	void AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem);
	void AddItemAtIndex(UInventoryItem* Item, int32 Index, bool bStackable, int32 StackAmount);
};
