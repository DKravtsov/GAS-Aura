﻿// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryGrid.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Editor/WidgetCompilerLog.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Widgets/Inventory/GridSlot/InventoryGridSlot.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverProxy.h"
#include "Widgets/Inventory/ItemPopUp/InventoryItemPopup.h"
#include "Widgets/Inventory/SlottedItems/InventorySlottedItemWidget.h"

void UInventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();

	InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInventoryGrid::AddItem);
	InventoryComponent->OnStackChanged.AddDynamic(this, &UInventoryGrid::OnStackChanged);
}

void UInventoryGrid::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	const FVector2D CanvasPos = UInventoryWidgetUtils::GetWidgetPosition(GridWidget);
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	const FVector2D CanvasSize = UInventoryWidgetUtils::GetWidgetSize(GridWidget);
	if (CursorExitedCanvas(CanvasPos, CanvasSize, MousePos))
	{
		return;
	}
	UpdateTileParameters(CanvasPos, MousePos);
}

bool UInventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location)
{
	bMouseWasWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UInventoryWidgetUtils::IsWithinBounds(BoundaryPos, BoundarySize, Location);
	if (!bMouseWithinCanvas && bMouseWasWithinCanvas)
	{
		UnHighlightSlots();
		return true;
	}
	return false;
}

void UInventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	if (!bMouseWithinCanvas)
		return;

	LastTileParameters = TileParameters;
	
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	TileParameters.TileCoordinates = HoveredTileCoordinates;
	TileParameters.TileIndex = GetIndexFromPosition(HoveredTileCoordinates);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);

	OnTileParametersUpdated(TileParameters);
}

void UInventoryGrid::OnTileParametersUpdated(const FInventoryTileParameters& Parameters)
{
	if (!IsValid(HoverItem))
		return;
	
	// Get Hover Item's dimensions
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	
	// calculate the starting coordinate for highlighting
	const FIntPoint StartCoords = CalculateStartingCoordinates(Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);
	ItemDropIndex = GetIndexFromPosition(StartCoords);

	// check hover position
	CurrentQueryResult = CheckHoverPosition(StartCoords, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots();
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		if (const auto GridFragment = UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(CurrentQueryResult.ValidItem.Get()))
		{
			ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EInventoryGridSlotVisualState::GrayedOut);
		}
	}
}

FInventorySpaceQueryResult UInventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions) const
{
	FInventorySpaceQueryResult Result;

	// in the grid bounds?
	const int32 GridIndex = GetIndexFromPosition(Position);
	if (!IsInGridBounds(GridIndex, Dimensions))
		return Result;

	Result.bHasSpace = true;
	
	// any items in the way?
	TSet<int32> OccupiedUpperLeftIndexes;
	UInventoryStatics::ForEach2D(GridSlots, GridIndex, Dimensions, Columns, [&](const UInventoryGridSlot* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndexes.Add(GridSlot->GetStartIndex());
			Result.bHasSpace = false;
		}
		return OccupiedUpperLeftIndexes.Num() <= 1; // early exit, if we found more items
	});
	
	// if so, is there only one item in the way? (can we swap?)
	if (OccupiedUpperLeftIndexes.Num() == 1)
	{
		const int32 Index = *OccupiedUpperLeftIndexes.FindArbitraryElement();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetStartIndex();
	}

	return Result;
}

void UInventoryGrid::HighlightSlots(const int32 StartIndex, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas)
		return;

	UnHighlightSlots();
	
	UInventoryStatics::ForEach2D(GridSlots, StartIndex, Dimensions, Columns, [](UInventoryGridSlot* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
		return true;
	});
	LastHighlightedIndex = StartIndex;
	LastHighlightedDimensions = Dimensions;
}

void UInventoryGrid::UnHighlightSlots(const int32 StartIndex, const FIntPoint& Dimensions)
{
	UInventoryStatics::ForEach2D(GridSlots, StartIndex, Dimensions, Columns, [](UInventoryGridSlot* GridSlot)
	{
		if (GridSlot->IsAvailable())
		{
			GridSlot->SetDefaultTexture();			
		}
		else
		{
			GridSlot->SetOccupiedTexture();
		}
		return true;
	});
	LastHighlightedIndex = INDEX_NONE;
}

void UInventoryGrid::ChangeHoverType(const int32 StartIndex, const FIntPoint& Dimensions, EInventoryGridSlotVisualState GridSlotState)
{
	UnHighlightSlots();
	UInventoryStatics::ForEach2D(GridSlots, StartIndex, Dimensions, Columns, [GridSlotState](UInventoryGridSlot* GridSlot)->bool
	{
		GridSlot->SetGridSlotState(GridSlotState);
		return true;
	});
	LastHighlightedIndex = StartIndex;
	LastHighlightedDimensions = Dimensions;
}

FIntPoint UInventoryGrid::CalculateStartingCoordinates(const FIntPoint& Coordinate, const FIntPoint& Dimensions, EInventoryTileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0;

	const FIntPoint HalfDimension = {FMath::FloorToInt32(Dimensions.X * 0.5f), FMath::FloorToInt32(Dimensions.Y * 0.5f)};

	FIntPoint StartingCoord;
	switch (Quadrant)
	{
	case EInventoryTileQuadrant::TopLeft:
		StartingCoord.X = Coordinate.X - HalfDimension.X;
		StartingCoord.Y = Coordinate.Y - HalfDimension.Y;
		break;
	case EInventoryTileQuadrant::TopRight:
		StartingCoord.X = Coordinate.X - HalfDimension.X + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - HalfDimension.Y;
		break;
	case EInventoryTileQuadrant::BottomLeft:
		StartingCoord.X = Coordinate.X - HalfDimension.X;
		StartingCoord.Y = Coordinate.Y - HalfDimension.Y + HasEvenHeight;
		break;
	case EInventoryTileQuadrant::BottomRight:
		StartingCoord.X = Coordinate.X - HalfDimension.X + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - HalfDimension.Y + HasEvenHeight;
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("Invalid quadrant"));
		StartingCoord = {-1, -1};
	}
	return StartingCoord;
}

FIntPoint UInventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// TODO: Make sure that TileSize is equal to actual Grid cell size
	
	const FIntPoint Coordinates{
		FMath::FloorToInt32((MousePosition.X - CanvasPosition.X) / TileSize),
		FMath::FloorToInt32((MousePosition.Y - CanvasPosition.Y) / TileSize)
	};
	return Coordinates;
}

EInventoryTileQuadrant UInventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// TODO: Make sure that TileSize is equal to actual Grid cell size
	
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, TileSize);

	const bool bInTop = TileLocalY < TileSize * 0.5f;
	const bool bInLeft = TileLocalX < TileSize * 0.5f;

	EInventoryTileQuadrant TileQuadrant;
	if (bInTop && bInLeft)
		TileQuadrant = EInventoryTileQuadrant::TopLeft;
	else if (bInTop && !bInLeft)
		TileQuadrant = EInventoryTileQuadrant::TopRight;
	else if (!bInTop && bInLeft)
		TileQuadrant = EInventoryTileQuadrant::BottomLeft;
	else
		TileQuadrant = EInventoryTileQuadrant::BottomRight;

	return TileQuadrant;
}

FInventorySlotAvailabilityResult UInventoryGrid::HasRoomForItem(const UInventoryItemComponent* ItemComponent) const
{
	check(ItemComponent);
	return HasRoomForItemInternal(ItemComponent->GetItemManifest());
}

FInventorySlotAvailabilityResult UInventoryGrid::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest, const int32 StackCountOverride) const
{
	FInventorySlotAvailabilityResult Result;

	// Determine if the item is stackable.Add commentMore actions
	const auto StackableFragment = ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	const auto GridFragment = ItemManifest.GetFragmentOfType<FInventoryItemGridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint{1,1};
	
	// Determine how many stacks to add.

	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? (StackCountOverride >= 0 ? StackCountOverride : StackableFragment->GetStackCount()) : 1;

	TSet<int32> CheckedIndexes;
	
	// For each Grid Slot:
	for (const auto& GridSlot : GridSlots)
	{
		// If we don't have anymore to fill, break out of the loop early.
		if (AmountToFill <= 0)
			break;
		
		// Is this index claimed yet?
		if (CheckedIndexes.Contains(GridSlot->GetTileIndex()))
			continue;
		
		// Can the item fit here? (i.e. is it out of grid bounds?)
		if (!IsInGridBounds(GridSlot->GetTileIndex(), Dimensions))
			continue;

		// Is there room at this index? (i.e. are there other items in the way?)
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, Dimensions, CheckedIndexes, TentativelyClaimed, MaxStackSize, ItemManifest.GetItemType()))
			continue;
		
		// How much to fill?
		const int32 AmountToFillInSlot =
			Result.bStackable ? CalculateStackableFillAmountForSlot(MaxStackSize, AmountToFill, GridSlot) : 1;
		
		if (AmountToFillInSlot <= 0)
			continue;
		
		CheckedIndexes.Append(TentativelyClaimed);
		
		// Update the amount left to fillAdd commentMore actions
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			GridSlot->GetInventoryItem().IsValid() ? GridSlot->GetStartIndex() : GridSlot->GetTileIndex(),
			Result.bStackable ? AmountToFillInSlot : 0,
			GridSlot->GetInventoryItem().IsValid()
			);

		AmountToFill -= AmountToFillInSlot;
		// How much is the Remainder?
		Result.Remainder = AmountToFill;
	}
	
	return Result;
}

bool UInventoryGrid::HasRoomAtIndex(const UInventoryGridSlot* GridSlot, const FIntPoint& Dimensions,
						const TSet<int32>& CheckedIndexes, TSet<int32>& OutTentativelyClaimedIndexes,
						const int32 MaxStackSize, const FGameplayTag& ItemType) const
{
	bool bHasRoomAtIndex = true;

	UInventoryStatics::ForEach2D(GridSlots, GridSlot->GetTileIndex(), Dimensions, Columns, [&](const UInventoryGridSlot* CurGridSlot)
	{
		if (CheckSlotConstraints(GridSlot, CurGridSlot, CheckedIndexes, OutTentativelyClaimedIndexes, MaxStackSize, ItemType))
		{
			OutTentativelyClaimedIndexes.Add(CurGridSlot->GetTileIndex());
			return true;
		}
		bHasRoomAtIndex = false;
		return false;
	});
	
	return bHasRoomAtIndex;
}

bool UInventoryGrid::CheckSlotConstraints(const UInventoryGridSlot* GridSlot,
										  const UInventoryGridSlot* CurGridSlot,
                                          const TSet<int32>& CheckedIndexes,
                                          TSet<int32>& OutTentativelyClaimedIndexes,
                                          const int32 MaxStackSize,
                                          const FGameplayTag& ItemType) const
{

	// Index claimed?
	if (CheckedIndexes.Contains(CurGridSlot->GetTileIndex()))
		return false;
	
	// Has valid item?
	if(!CurGridSlot->GetInventoryItem().IsValid())
	{
		OutTentativelyClaimedIndexes.Add(CurGridSlot->GetTileIndex());
		return true;
	}

	// Is this grid slot an upper left slot? (we care only about upper left slot because only there our stack stored)
	if (CurGridSlot->GetStartIndex() != GridSlot->GetTileIndex())
		return false; 
	
	// If so, is this a stackable item?
	const UInventoryItem* CurItem = CurGridSlot->GetInventoryItem().Get();
	if (!CurItem->IsStackable())
		return false;
	
	// Is this item the same type as the item we're trying to add?
	if (!CurItem->GetItemType().MatchesTagExact(ItemType))
		return false;

	// If stackable, is this slot at the max stack size already?
	if (GridSlot->GetStackCount() >= MaxStackSize)
		return false;
	
	return true;
}

bool UInventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& Dimensions) const
{
	if (!GridSlots.IsValidIndex(StartIndex))
		return false;
	const int32 EndColumn = StartIndex % Columns + Dimensions.X;
	const int32 EndRow = StartIndex / Columns + Dimensions.Y;

	return EndColumn <= Columns && EndRow <= Rows;
}

int32 UInventoryGrid::CalculateStackableFillAmountForSlot(const int32 MaxStackSize,
                                                          const int32 Amount, const UInventoryGridSlot* GridSlot) const
{
	const int32 RooInSlot = MaxStackSize - GetStackAmountInSlot(GridSlot);
	return FMath::Min(Amount, RooInSlot);
}

int32 UInventoryGrid::GetStackAmountInSlot(const UInventoryGridSlot* GridSlot) const
{
	check(GridSlot);
	int32 StackCount = GridSlot->GetStackCount();
	if (const int32 UpperLeftIndex = GridSlot->GetStartIndex(); UpperLeftIndex != INDEX_NONE)
	{
		const UInventoryGridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		StackCount = UpperLeftGridSlot->GetStackCount();
	}
	return StackCount;
}

void UInventoryGrid::AddItem(UInventoryItem* Item)
{
	if (!IsValid(Item) || !MatchesCategory(Item))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Adding item: [%s]"), *GetNameSafe(Item));

	const auto Result = HasRoomForItemInternal(Item->GetItemManifest());
	AddItemToIndexes(Result, Item);
}

void UInventoryGrid::AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.Amount);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.Amount);
	}
}

void UInventoryGrid::AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable,	const int32 StackAmount)
{
	const auto GridFragment = UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(Item);
	if (GridFragment == nullptr)
		return;
	const auto ImageFragment = UInventoryWidgetUtils::GetImageFragmentFromInventoryItem(Item);
	if (ImageFragment == nullptr)
		return;

	UInventorySlottedItemWidget* SlottedItem = CreateSlottedItemWidget(Item, Index, *GridFragment, *ImageFragment, bStackable, StackAmount);
	AddSlottedItemToGrid(Index, *GridFragment, SlottedItem);
	SlottedItems.Add(Index, SlottedItem);
}

UInventorySlottedItemWidget* UInventoryGrid::CreateSlottedItemWidget(UInventoryItem* Item, const int32 Index,
                                                                     const FInventoryItemGridFragment& GridFragment,
                                                                     const FInventoryItemImageFragment& ImageFragment,
                                                                     bool bStackable, const int32 StackAmount)
{
	UInventorySlottedItemWidget* SlottedItem =
		CreateWidget<UInventorySlottedItemWidget>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 NewStackAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(NewStackAmount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &UInventoryGrid::OnSlottedItemClicked);
	
	return SlottedItem;
}

void UInventoryGrid::SetSlottedItemImage(const UInventorySlottedItemWidget* SlottedItem, const FInventoryItemGridFragment& GridFragment, const FInventoryItemImageFragment& ImageFragment) const
{
	SlottedItem->SetImageBrush(GetTempBrush());
	
	const FVector2D DrawSize = UInventoryWidgetUtils::GetDrawSize(GridFragment, TileSize);
	ImageFragment.GetIcon().LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateLambda([DrawSize, SlottedItem](const FSoftObjectPath& SoftPath, UObject* LoadedObject)
	{
		if (!IsValid(LoadedObject))
		{
			UE_LOG(LogInventory, Error, TEXT("Loading failed: [%s]"), *SoftPath.ToString());
			return;
		}
		FSlateBrush ImageBrush;
		ImageBrush.SetResourceObject(LoadedObject);
		ImageBrush.DrawAs = ESlateBrushDrawType::Image;
		ImageBrush.ImageSize = DrawSize;
		SlottedItem->SetImageBrush(ImageBrush);
	}));
}

FSlateBrush UInventoryGrid::GetTempBrush()
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = FVector2D{1,1};
	return Brush;
}

void UInventoryGrid::PutHoverItemDown()
{
	if (!HasHoverItem())
		return;

	FInventorySlotAvailabilityResult Result = HasRoomForItemInternal(HoverItem->GetInventoryItem()->GetItemManifest(), HoverItem->GetStackCount());
	Result.Item = HoverItem->GetInventoryItem();

	OnStackChanged(Result);

	ClearHoverItem();
}

void UInventoryGrid::AddSlottedItemToGrid(const int32 Index, const FInventoryItemGridFragment& GridFragment, UInventorySlottedItemWidget* SlottedItem) const
{
	const FIntPoint Pos = GetPositionFromIndex(Index);
	if (const auto GridSlot = GridWidget->AddChildToGrid(SlottedItem, Pos.Y, Pos.X))
	{
		GridSlot->SetColumnSpan(GridFragment.GetGridSize().X);
		GridSlot->SetRowSpan(GridFragment.GetGridSize().Y);
	}
}

void UInventoryGrid::UpdateGridSlots(UInventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackable)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}

	const FInventoryItemGridFragment* GridFragment = UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(NewItem);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint{1,1};

	UInventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns,
		[&](UInventoryGridSlot* GridSlot)-> bool
		{
			GridSlot->SetInventoryItem(NewItem);
			GridSlot->SetStartIndex(Index);
			GridSlot->SetOccupiedTexture();
			GridSlot->SetIsAvailable(false);
			return true;
		});
}

void UInventoryGrid::ConstructGrid()
{
	check(Rows > 0 && Columns > 0);
	
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

			InventoryGridSlot->OnGridSlotClicked.AddDynamic(this, &UInventoryGrid::OnGridSlotClicked);
			InventoryGridSlot->OnGridSlotHovered.AddDynamic(this, &UInventoryGrid::OnGridSlotHovered);
			InventoryGridSlot->OnGridSlotUnhovered.AddDynamic(this, &UInventoryGrid::OnGridSlotUnhovered);
		}
	}
}

bool UInventoryGrid::MatchesCategory(const UInventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory().MatchesTag(ItemCategory);
}

void UInventoryGrid::OnStackChanged(const FInventorySlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get()))
		return;

	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			const int32 NewStackCount = GridSlot->GetStackCount() + Availability.Amount;
			SlottedItem->UpdateStackCount(NewStackCount);
			GridSlot->SetStackCount(NewStackCount);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.Amount);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.Amount);
		}
	}
}

bool UInventoryGrid::IsLeftMouseButtonClick(const FPointerEvent& MouseEvent)
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

bool UInventoryGrid::IsRightMouseButtonClick(const FPointerEvent& MouseEvent)
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

void UInventoryGrid::PickUpItemInInventory(UInventoryItem* ClickedItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedItem, GridIndex, GridIndex);

	RemoveItemFromGrid(ClickedItem, GridIndex);
}

void UInventoryGrid::PutDownItemInInventoryAtIndex(const int32 GridIndex)
{
	check(IsValid(HoverItem));
	AddItemAtIndex(HoverItem->GetInventoryItem(), GridIndex, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), GridIndex, HoverItem->IsStackable(), HoverItem->GetStackCount());

	ClearHoverItem();
}

void UInventoryGrid::ShowDefaultCursor() const
{
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, nullptr);
}

void UInventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem))
		return;

	HoverItem->Reset();
	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	ShowDefaultCursor();
}

void UInventoryGrid::SwapWithHoverItem(UInventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem))
		return;

	UInventoryItem* TempItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempStackable = HoverItem->IsStackable();
	// keep the same PreviousGridIndex

	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	AddItemAtIndex(TempItem, ItemDropIndex, bTempStackable, TempStackCount);
	UpdateGridSlots(TempItem, ItemDropIndex, bTempStackable, TempStackCount);
}

void UInventoryGrid::AssignHoverItem(UInventoryItem* ClickedItem, const int32 GridIndex, const int32 PrevGridIndex)
{
	const auto GridFragment = UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(ClickedItem);
	if (GridFragment == nullptr)
		return;
	const auto ImageFragment = UInventoryWidgetUtils::GetImageFragmentFromInventoryItem(ClickedItem);
	if (ImageFragment == nullptr)
		return;

	if(!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInventoryHoverProxy>(GetOwningPlayer(), HoverItemClass);
	}

	const FVector2D DrawSize = UInventoryWidgetUtils::GetDrawSize(*GridFragment, TileSize) * UWidgetLayoutLibrary::GetViewportScale(this);;

	ImageFragment->GetIcon().LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateLambda([DrawSize, this](const FSoftObjectPath& SoftPath, UObject* LoadedObject)
	{
		if (!IsValid(LoadedObject))
		{
			UE_LOG(LogInventory, Error, TEXT("Loading failed: [%s]"), *SoftPath.ToString());
			return;
		}
		FSlateBrush ImageBrush;
		ImageBrush.SetResourceObject(LoadedObject);
		ImageBrush.DrawAs = ESlateBrushDrawType::Image;
		ImageBrush.ImageSize = DrawSize;
		HoverItem->SetImageBrush(ImageBrush);
	}));

	HoverItem->SetImageBrush(GetTempBrush());
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(ClickedItem);
	HoverItem->SetIsStackable(ClickedItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);

	if (GridIndex != INDEX_NONE)
	{
		HoverItem->UpdateStackCount(ClickedItem->IsStackable()? GridSlots[GridIndex]->GetStackCount() : 0);
	}
	if (PrevGridIndex != INDEX_NONE)
	{
		HoverItem->SetPreviousGridIndex(PrevGridIndex);
	}
}

void UInventoryGrid::OnHide()
{
	PutHoverItemDown();
	ShowDefaultCursor();
}

void UInventoryGrid::RemoveItemFromGrid(UInventoryItem* ClickedItem, const int32 GridIndex)
{
	const auto GridFragment = UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(ClickedItem);
	if (GridFragment == nullptr)
		return;

	UInventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [](UInventoryGridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetStartIndex(INDEX_NONE);
		GridSlot->SetDefaultTexture();
		GridSlot->SetIsAvailable(true);
		GridSlot->SetStackCount(0);
		return true;
	});

	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UInventorySlottedItemWidget> FoundSlottedItem;
		if (SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem))
		{
			FoundSlottedItem->RemoveFromParent();
		}
	}
}

void UInventoryGrid::OnGridSlotClicked(int32 GridSlotIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem) || !GridSlots.IsValidIndex(ItemDropIndex))
		return;

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	if (!IsInGridBounds(ItemDropIndex, HoverItem->GetGridDimensions()))
		return;

	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownItemInInventoryAtIndex(ItemDropIndex);
	}
}

void UInventoryGrid::OnGridSlotHovered(int32 GridSlotIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem))
		return;

	const auto GridSlot = GridSlots[GridSlotIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UInventoryGrid::OnGridSlotUnhovered(int32 GridSlotIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem))
		return;

	const auto GridSlot = GridSlots[GridSlotIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetDefaultTexture();
	}
}

bool UInventoryGrid::IsHoverItemSameStackableAs(const UInventoryItem* ClickedInventoryItem) const
{
	const bool bSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemType());
}

int32 UInventoryGrid::GetMaxStackSize(const UInventoryItem* Item)
{
	check(IsValid(Item));
	if (const auto StackableFragment = Item->GetItemManifest().GetFragmentOfType<FInventoryItemStackableFragment>())
	{
		return StackableFragment->GetMaxStackSize();
	}
	return 1;
}

void UInventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UE_LOG(LogTemp, Warning, TEXT("Clicked on item: %d"), GridIndex);

	UInventoryStatics::ItemUnhovered(GetOwningPlayer());
	
	UInventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem))
	{
		if (IsLeftMouseButtonClick(MouseEvent))
		{
			PickUpItemInInventory(ClickedInventoryItem, GridIndex);
		}
		else if (IsRightMouseButtonClick(MouseEvent))
		{
			CreateItemPopupMenu(GridIndex);
		}
		return;
	}

	// Do the hovered item and the clicked inventory item share a type, and are they stackable?
	if (IsHoverItemSameStackableAs(ClickedInventoryItem))
	{
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		const int32 MaxStackSize = GetMaxStackSize(ClickedInventoryItem);
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		
		// Should we swap their stack counts?
		if (ClickedStackCount == MaxStackSize && HoveredStackCount < MaxStackSize)
		{
			SwapStackCountsWithHoverItem(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		FillInStacksOrConsumeHover(ClickedStackCount, HoveredStackCount, MaxStackSize, GridIndex);
		return;
	}

	if (CurrentQueryResult.ValidItem.IsValid())
	{
		// Swap with the hover item.
		SwapWithHoverItem(ClickedInventoryItem, GridIndex);
	}
}

void UInventoryGrid::CreateItemPopupMenu(const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));

	if (IsValid(ItemPopupMenu))
	{
		if (ItemPopupMenu->GetGridIndex() != GridIndex)
		{
			ItemPopupMenu->HideMenu();
		}
		else
		{
			return;
		}
	}
	
	if (UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get())
	{
		ItemPopupMenu = CreateWidget<UInventoryItemPopup>(GetOwningPlayer(), ItemPopupMenuClass);
		check(IsValid(ItemPopupMenu));
		ItemPopupMenu->SetGridIndex(GridIndex);

		UCanvasPanelSlot* CanvasSlot = OwningCanvasPanel->AddChildToCanvas(ItemPopupMenu);
		UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopupMenu);
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
		CanvasSlot->SetPosition(MousePosition - PopupMenuOffset);
		CanvasSlot->SetSize(ItemPopupMenu->GetBoxSize());


		const int32 StackCount = GridSlots[GridIndex]->GetStackCount();
		if (StackCount >= 2)
		{
			ItemPopupMenu->SetSliderParams(-1, StackCount);
			ItemPopupMenu->OnSplitDelegate.BindUObject(this, &ThisClass::OnPopupMenuSplit);
		}
		else
		{
			ItemPopupMenu->CollapseSplitButton();
		}

		ItemPopupMenu->OnDropDelegate.BindUObject(this, &ThisClass::OnPopupMenuDrop);

		if (RightClickedItem->IsConsumable())
		{
			ItemPopupMenu->OnConsumeDelegate.BindUObject(this, &ThisClass::OnPopupMenuConsume);
		}
		else
		{
			ItemPopupMenu->CollapseConsumeButton();
		}
	}
}

void UInventoryGrid::SwapStackCountsWithHoverItem(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 GridIndex)
{
	UpdateStackCountInSlot(GridIndex, HoveredStackCount);
	HoverItem->UpdateStackCount(ClickedStackCount);
}

void UInventoryGrid::FillInStacksOrConsumeHover(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 MaxStackCount, const int32 GridIndex)
{
	const int32 FillAmount = FMath::Min(HoveredStackCount, MaxStackCount - ClickedStackCount);
	if (FillAmount == 0)
		return;
	
	const int32 NewStackCount = ClickedStackCount + FillAmount;
	const int32 Remainder = HoveredStackCount - FillAmount;

	UpdateStackCountInSlot(GridIndex, NewStackCount);

	if (Remainder == 0)
	{
		ClearHoverItem();
		ShowDefaultCursor();

		const auto GridFragment = UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(GridSlots[GridIndex]->GetInventoryItem().Get());
		const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint{1,1};
		HighlightSlots(GridIndex, Dimensions);
	}
	else
	{
		HoverItem->UpdateStackCount(Remainder);
	}
}

void UInventoryGrid::UpdateStackCountInSlot(const int32 GridIndex, const int32 NewStackCount)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInventoryGridSlot* GridSlot = GridSlots[GridIndex];
	GridSlot->SetStackCount(NewStackCount);
	
	UInventorySlottedItemWidget* SlottedItem = SlottedItems.FindChecked(GridIndex);
	SlottedItem->UpdateStackCount(NewStackCount);
}

void UInventoryGrid::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}

void UInventoryGrid::OnPopupMenuSplit(const int32 SplitAmount, const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	check(RightClickedItem && RightClickedItem->IsStackable());
	const int32 UpperLeftIndex =  GridSlots[GridIndex]->GetStartIndex();
	const int32 NewStackCount = GridSlots[UpperLeftIndex]->GetStackCount() - SplitAmount;
	
	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	check(IsValid(HoverItem));
	HoverItem->UpdateStackCount(SplitAmount);
	
	UpdateStackCountInSlot(UpperLeftIndex, NewStackCount);
}

void UInventoryGrid::OnPopupMenuConsume(const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));
	check(InventoryComponent.IsValid());
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if( !IsValid(RightClickedItem))
		return;
	check(RightClickedItem->IsConsumable());

	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetStartIndex();
	const int32 NewStackCount = GridSlots[UpperLeftIndex]->GetStackCount() - 1;
	UpdateStackCountInSlot(UpperLeftIndex, NewStackCount);
	
	InventoryComponent->ConsumeItem(RightClickedItem);

	if (NewStackCount <= 0)
	{
		RemoveItemFromGrid(RightClickedItem, UpperLeftIndex);
	}
}

void UInventoryGrid::OnPopupMenuDrop(const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if( !IsValid(RightClickedItem))
		return;
	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetStartIndex();
	PickUpItemInInventory(RightClickedItem, UpperLeftIndex);
	DropHoverItemOnGround();
}

void UInventoryGrid::DropHoverItemOnGround()
{
	if (!IsValid(HoverItem) || !IsValid(HoverItem->GetInventoryItem()))
		return;

	InventoryComponent->DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());

	ClearHoverItem();
	ShowDefaultCursor();
}

bool UInventoryGrid::HasHoverItem() const
{
	return IsValid(HoverItem);
}

#if WITH_EDITOR
void UInventoryGrid::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (Rows == 0 || Columns == 0)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(": Rows and Columns expected to be set.")));
	}

	if (TileSize == 0.f)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(": TileSize must be set. It should be equal to GridSlot's size.")));
	}

	if (!GridSlotClass)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(" has no GridSlotClass specified.")));
	}
	if (!SlottedItemClass)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(" has no SlottedItemClass specified.")));
	}
	if (!HoverItemClass)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(" has no HoverItemClass specified.")));
	}
	if (!ItemPopupMenuClass)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(" has no ItemPopupMenuClass specified.")));
	}
}
#endif//WITH_EDITOR

