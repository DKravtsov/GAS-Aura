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

};
