// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryGrid.h"

#include "Components/UniformGridPanel.h"
#include "Widgets/Inventory/GridSlot/InventoryGridSlot.h"

void UInventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();
}

void UInventoryGrid::ConstructGrid()
{
	check(Rows > 0 && Columns > 0 && TileSize > 0.f);
	
	GridSlots.Reserve(Rows * Columns);

	GridWidget->ClearChildren();
	GridWidget->SetMinDesiredSlotWidth(TileSize);
	GridWidget->SetMinDesiredSlotHeight(TileSize);

	for (int RowIndex = 0; RowIndex < Rows; RowIndex++)
	{
		for (int ColumnIndex = 0; ColumnIndex < Columns; ColumnIndex++)
		{
			UInventoryGridSlot* InventoryGridSlot = CreateWidget<UInventoryGridSlot>(this, GridSlotClass);
			
			GridWidget->AddChildToUniformGrid(InventoryGridSlot, RowIndex, ColumnIndex);
			
			const int32 AddedIndex = GridSlots.Add(InventoryGridSlot);
			check(AddedIndex == GetIndexFromPosition(FIntPoint(ColumnIndex, RowIndex)));
			InventoryGridSlot->SetTileIndex(AddedIndex);
		}
	}
}
