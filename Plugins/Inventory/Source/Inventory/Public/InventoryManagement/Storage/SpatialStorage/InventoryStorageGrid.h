// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "InventoryManagement/FastArray/InventoryStorageGridFastArray.h"
#include "UObject/Object.h"
#include "InventoryStorageGrid.generated.h"

struct FInventoryItemManifest;
/**
 * 
 */
UCLASS(MinimalAPI, BlueprintType)
class UInventoryStorageGrid : public UObject
{
	GENERATED_BODY()

public:

	FInventoryItemGridChangedDelegate OnItemAddedToGrid;
	FInventoryItemChangedDelegate OnItemRemoved;
	FInventoryItemGridChangedDelegate OnStackChanged;
	FInventoryGridSlotsUpdatedSignature OnGridSlotsUpdated;
	FInventoryGridSlotsUpdatedSignature OnGridSlotsReset;

private:
	
	UPROPERTY(ReplicatedUsing=OnRep_GridSlots)
	FInventoryStorageGridFastArray GridSlots;

	UPROPERTY(Replicated)
	int32 Rows = 1;

	UPROPERTY(Replicated)
	int32 Columns = 1;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.ItemCategory"), Replicated)
	FGameplayTag ItemCategory;

	mutable TWeakObjectPtr<class UInventoryComponent> InventoryComponent;
	mutable TWeakObjectPtr<AActor> OwningActor;

public:

	UInventoryStorageGrid();
	
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

	AActor* GetOwningActor() const;

	int32 GetRows() const {return Rows;}
	int32 GetColumns() const {return Columns;}
	TArray<FInventoryStorageGridSlot> GetGridSlotsCopy() const {return GridSlots.GetAllSlots();}
	const FInventoryStorageGridSlot& GetGridSlot(int32 Index) const;
	const FInventoryStorageGridSlot& GetGridSlot(const FIntPoint& Position) const;

	bool ContainsItem(UInventoryItem* Item) const;
	int32 GetItemIndex(UInventoryItem* Item) const;

	bool IsValidIndex(int32 Index) const {return GridSlots.IsValidIndex(Index);}

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

	void NotifyGridChanged(TArrayView<FPlatformTypes::int32> ChangedIndices);

	int32 GetStackCount(int32 GridIndex) const;
	void SetStackCount(int32 GridIndex, int32 NewStackCount);

	// returns remainder
	int32 FillInStacksOrConsumeHover(UInventoryItem* Item, int32 TargetIndex, int32 AddStackCount);

private:
	void ConstructGrid();
	void InitOwner() const;

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

	bool MatchesCategory(const UInventoryItem* Item) const;

	void AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem);
	void AddItemAtIndex(UInventoryItem* Item, int32 Index, bool bStackable, int32 StackAmount);

	UFUNCTION()
	void OnRep_GridSlots();
};
