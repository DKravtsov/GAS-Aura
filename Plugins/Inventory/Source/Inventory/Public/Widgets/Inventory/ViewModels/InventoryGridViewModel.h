// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryGridTypes.h"
#include "InventoryGridViewModel.generated.h"

class UInventoryGridWidget;
class UInventoryStorageGrid;
struct FGameplayTag;
struct FInventoryStorageGridSlot;
struct FInventorySlotAvailabilityResult;
class UInventoryComponent;
class UInventoryItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FInventoryItemGridSignature, UInventoryItem* /*ItemToRemove*/, int32 /*GridIndex*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryGridSlotUpdatedSignature, int32 /*GridIndex*/);

/**
 * Intermediary between InventoryGridWidget(View) and InventorySpatialStorageGrid(Model)
 */
UCLASS(MinimalAPI)
class UInventoryGridViewModel : public UObject
{
	GENERATED_BODY()

public:

	FInventoryItemGridChangedDelegate& GetOnItemAddedToGridDelegate() const;
	FInventoryItemChangedDelegate& GetOnItemRemovedDelegate() const;
	FInventoryItemGridChangedDelegate& GetOnStackChangedDelegate() const;
	FInventoryGridSlotsUpdatedSignature& GetOnGridSlotsUpdatedDelegate() const;
	FInventoryGridSlotsUpdatedSignature& GetOnGridSlotsResetDelegate() const;

private:

	TWeakObjectPtr<UInventoryStorageGrid> StorageGrid;
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;

public:

	void Initialize(UInventoryGridWidget* InGridWidget, const FGameplayTag& ItemCategory);

	int32 GetRows() const;

	int32 GetColumns() const;

	const FGameplayTag& GetInventoryCategory() const;

	UInventoryComponent* GetInventoryComponent() const;

	TWeakObjectPtr<UInventoryStorageGrid> GetStorageGrid() const {return StorageGrid;};

	const FInventoryStorageGridSlot& GetGridSlot(int32 Index) const;

	bool IsGridSlotAvailable(int32 Index) const;

	void UpdateStackCount(int32 Index, int32 NewStackCount) const;

	void UpdateGridSlots(UInventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount) const;
	void RemoveItemFromGrid(UInventoryItem* ItemToRemove, const int32 GridIndex) const;

	void AssignHoverItem(UInventoryItem* InventoryItem, int32 GridIndex, int32 PrevGridIndex);
	void ClearHoverItem();
	// puts hover item back into inventory and clears the hover item
	void PutDownHoverItem(const FInventorySlotAvailabilityResult& Result);
	void FillInStacksOrConsumeHover(UInventoryItem* Item, int32 TargetIndex, int32 SourceIndex);
	// Split <SplitAmount> stack from stackable item in slot to HoverItem
	void SplitStackToHoverItem(UInventoryItem* InventoryItem, int32 GridIndex, int32 SplitAmount);
	void SwapStackCountWithHoverItem(UInventoryItem* InventoryItem, int32 GridIndex);
private:

	bool HasAuthority() const;

};
