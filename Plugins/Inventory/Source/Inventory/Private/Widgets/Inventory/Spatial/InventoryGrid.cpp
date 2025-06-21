// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryGrid.h"

#include "Components/UniformGridPanel.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Widgets/Inventory/GridSlot/InventoryGridSlot.h"
#include "Widgets/Inventory/SlottedItems/InventorySlottedItemWidget.h"

void UInventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();

	InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInventoryGrid::AddItem);
}

FInventorySlotAvailabilityResult UInventoryGrid::HasRoomForItem(const UInventoryItemComponent* ItemComponent) const
{
	check(ItemComponent);
	return HasRoomForItemInternal(ItemComponent->GetItemManifest());
}

FInventorySlotAvailabilityResult UInventoryGrid::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest) const
{
	// TODO: implement this properly
	
	FInventorySlotAvailabilityResult Result;
	Result.TotalRoomToFill = 1;

	FInventorySlotAvailability SlotAvailability;
	SlotAvailability.Index = 0;
	SlotAvailability.Amount = 1;
	SlotAvailability.bItemAtIndex = false;

	Result.SlotAvailabilities.Add(MoveTemp(SlotAvailability));
	
	return Result;
}

void UInventoryGrid::AddItem(UInventoryItem* Item)
{
	if (!IsValid(Item) || !MatchesCategory(Item))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Adding item: [%s]"), *GetNameSafe(Item));

	auto Result = HasRoomForItemInternal(Item->GetItemManifest());
	AddItemToIndexes(Result, Item);
}

void UInventoryGrid::AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.Amount);
	}
}

void UInventoryGrid::AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable,	const int32 StackAmount)
{
	const auto GridFragment = UInventoryItem::GetFragment<FInventoryItemGridFragment>(Item, InventoryFragmentTags::FragmentTag_Grid);
	if (GridFragment == nullptr)
		return;
	const auto ImageFragment = UInventoryItem::GetFragment<FInventoryItemImageFragment>(Item, InventoryFragmentTags::FragmentTag_Image);
	if (ImageFragment == nullptr)
		return;

	UInventorySlottedItemWidget* SlottedItem = CreateSlottedItemWidget(Item, Index, *GridFragment, *ImageFragment);
	AddSlottedItemToGrid(Index, *GridFragment, SlottedItem);
	SlottedItems.Add(Index, SlottedItem);
}

UInventorySlottedItemWidget* UInventoryGrid::CreateSlottedItemWidget(UInventoryItem* Item, const int32 Index,
                                                                     const FInventoryItemGridFragment& GridFragment,
                                                                     const FInventoryItemImageFragment& ImageFragment)
{
	UInventorySlottedItemWidget* SlottedItem =
		CreateWidget<UInventorySlottedItemWidget>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	return SlottedItem;
}

FVector2D UInventoryGrid::GetDrawSize(const FInventoryItemGridFragment& GridFragment) const
{
	const float IconTileSize = TileSize - GridFragment.GetGridPadding() * 2; 
	return GridFragment.GetGridSize() * IconTileSize;
}

void UInventoryGrid::SetSlottedItemImage(const UInventorySlottedItemWidget* SlottedItem, const FInventoryItemGridFragment& GridFragment, const FInventoryItemImageFragment& ImageFragment) const
{
	FSlateBrush ImageBrush;
	// ToDo: async loading
	ImageBrush.SetResourceObject(ImageFragment.GetIcon().LoadSynchronous());
	ImageBrush.DrawAs = ESlateBrushDrawType::Image;
	ImageBrush.ImageSize = GetDrawSize(GridFragment);

	SlottedItem->SetImageBrush(ImageBrush);
}

void UInventoryGrid::AddSlottedItemToGrid(const int32 Index, const FInventoryItemGridFragment& GridFragment,
	UInventorySlottedItemWidget* SlottedItem) const
{
	FIntPoint Pos = GetPositionFromIndex(Index);
	if (auto GridSlot = GridWidget->AddChildToGrid(SlottedItem, Pos.X, Pos.Y))
	{
		GridSlot->SetColumnSpan(GridFragment.GetGridSize().X);
		GridSlot->SetRowSpan(GridFragment.GetGridSize().Y);
	}
}

void UInventoryGrid::ConstructGrid()
{
	check(Rows > 0 && Columns > 0 && TileSize > 0.f);
	
	GridSlots.Reserve(Rows * Columns);

	GridWidget->ClearChildren();

	for (int RowIndex = 0; RowIndex < Rows; RowIndex++)
	{
		for (int ColumnIndex = 0; ColumnIndex < Columns; ColumnIndex++)
		{
			UInventoryGridSlot* InventoryGridSlot = CreateWidget<UInventoryGridSlot>(this, GridSlotClass);
			
			GridWidget->AddChildToGrid(InventoryGridSlot, RowIndex, ColumnIndex);
			
			const int32 AddedIndex = GridSlots.Add(InventoryGridSlot);
			check(AddedIndex == GetIndexFromPosition(FIntPoint(ColumnIndex, RowIndex)));
			InventoryGridSlot->SetTileIndex(AddedIndex);
		}
	}
}

bool UInventoryGrid::MatchesCategory(const UInventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory().MatchesTag(ItemCategory);
}
