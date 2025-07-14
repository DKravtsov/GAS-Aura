// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "InventorySpatialStorage.generated.h"

class UInventoryStorageGrid;

/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable)
class UInventorySpatialStorage : public UInventoryStorage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(UIMin=1, ClampMin=1))
	int32 Rows = 1;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(UIMin=1, ClampMin=1))
	int32 Columns = 1;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.ItemCategory"))
	TArray<FGameplayTag> GridCategories;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UInventoryStorageGrid>> InventoryGrids;

public:

	AActor* GetOwningActor() const;

	virtual void SetupStorage() override;

	UInventoryStorageGrid* FindInventoryGridByCategory(const FGameplayTag& ItemCategory) const; 

	//~ Begin of UInventoryStorage interface
	virtual int32 GetItemIndex(UInventoryItem* Item) override;
	virtual void UpdateGridSlots(UInventoryItem* NewItem, int32 Index, bool bStackable, int32 StackAmount) override;
	virtual void RemoveItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex) override;
	//~ End of UInventoryStorage interface

	//~ Begin of UObject interface
	virtual bool IsSupportedForNetworking() const override {return true;}
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End of UObject interface

	//#if UE_WITH_CHEAT_MANAGER
	virtual void DebugPrintStorage() const override;
	//#endif//UE_WITH_CHEAT_MANAGER
protected:

	static TSubclassOf<UInventoryStorageGrid> GetStorageGridClass();

	virtual FInventorySlotAvailabilityResult HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const override;
};
