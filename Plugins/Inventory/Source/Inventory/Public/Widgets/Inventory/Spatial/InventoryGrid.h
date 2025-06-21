// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridTypes.h"
#include "InventoryGrid.generated.h"


class UInventoryGridSlot;
class UUniformGridPanel;

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
	int32 Rows = 0;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns = 0;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float TileSize = 0.f;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridWidget;

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

	UFUNCTION()
	void AddItem(UInventoryItem* Item);
	
private:

	void ConstructGrid();
	bool MatchesCategory(const UInventoryItem* Item) const;
};
