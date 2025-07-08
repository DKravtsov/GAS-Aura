// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryGridTypes.h"
#include "InventoryGridViewModel.generated.h"

class UInventorySpatialStorageGrid;
struct FGameplayTag;
struct FInventoryStorageGridSlot;
struct FInventorySlotAvailabilityResult;
class UInventoryComponent;
class UInventoryItem;


/**
 * Intermediary between InventoryGridWidget(View) and InventorySpatialStorageGrid(Model)
 */
UCLASS(MinimalAPI)
class UInventoryGridViewModel : public UObject
{
	GENERATED_BODY()

public:

	FInventoryItemGridChangedDelegate OnItemAdded;
	FInventoryItemChangedDelegate OnItemRemoved;
	FInventoryItemGridChangedDelegate OnStackChanged;

private:

	TWeakObjectPtr<UInventorySpatialStorageGrid> StorageGrid;
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;

public:

	void Initialize(UInventoryComponent* InInventoryComponent, const FGameplayTag& ItemCategory);
	void Initialize(UInventoryComponent* InInventoryComponent, UInventorySpatialStorageGrid* InGrid);

	int32 GetRows() const;

	int32 GetColumns() const;

	const FGameplayTag& GetInventoryCategory() const;

	UInventoryComponent* GetInventoryComponent() const;

	const TArray<FInventoryStorageGridSlot>& GetGridSlots() const;
	const FInventoryStorageGridSlot& GetGridSlot(int32 Index) const;
	FInventoryStorageGridSlot& GetGridSlotMutable(int32 Index);

private:

	// subscribe to corresponding events from InventorySpatialStorageGrid
	//?
};
