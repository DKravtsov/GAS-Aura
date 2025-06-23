// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryGrid.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
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
		if (const auto GridFragment =GetGridFragmentFromInventoryItem(CurrentQueryResult.ValidItem.Get()))
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
		return OccupiedUpperLeftIndexes.Num() <= 1; // early exit, if we found more than 1 items
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

FInventorySlotAvailabilityResult UInventoryGrid::HasRoomForItemInternal(const FInventoryItemManifest& ItemManifest) const
{
	FInventorySlotAvailabilityResult Result;

	// Determine if the item is stackable.Add commentMore actions
	const auto StackableFragment = ItemManifest.GetFragmentOfType<FInventoryItemStackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	const auto GridFragment = ItemManifest.GetFragmentOfType<FInventoryItemGridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint{1,1};
	
	// Determine how many stacks to add.

	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;

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

	auto Result = HasRoomForItemInternal(Item->GetItemManifest());
	AddItemToIndexes(Result, Item);
}

void UInventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UE_LOG(LogTemp, Warning, TEXT("Clicked on item: %d"), GridIndex);
	UInventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem) && IsLeftMouseButtonClick(MouseEvent))
	{
		PickUpItemInInventory(ClickedInventoryItem, GridIndex);
	}
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
	const auto GridFragment = GetGridFragmentFromInventoryItem(Item);
	if (GridFragment == nullptr)
		return;
	const auto ImageFragment = GetImageFragmentFromInventoryItem(Item);
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
	const FIntPoint Pos = GetPositionFromIndex(Index);
	if (const auto GridSlot = GridWidget->AddChildToGrid(SlottedItem, Pos.Y, Pos.X))
	{
		GridSlot->SetColumnSpan(GridFragment.GetGridSize().X);
		GridSlot->SetRowSpan(GridFragment.GetGridSize().Y);
	}
}

const FInventoryItemGridFragment* UInventoryGrid::GetGridFragmentFromInventoryItem(UInventoryItem* NewItem)
{
	return UInventoryItem::GetFragment<FInventoryItemGridFragment>(NewItem, InventoryFragmentTags::FragmentTag_Grid);
}

const FInventoryItemImageFragment* UInventoryGrid::GetImageFragmentFromInventoryItem(UInventoryItem* Item)
{
	return UInventoryItem::GetFragment<FInventoryItemImageFragment>(Item, InventoryFragmentTags::FragmentTag_Image);
}

void UInventoryGrid::UpdateGridSlots(UInventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackable)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}

	const FInventoryItemGridFragment* GridFragment = GetGridFragmentFromInventoryItem(NewItem);
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

void UInventoryGrid::AssignHoverItem(UInventoryItem* ClickedItem, const int32 GridIndex, const int32 PrevGridIndex)
{
	const auto GridFragment = GetGridFragmentFromInventoryItem(ClickedItem);
	if (GridFragment == nullptr)
		return;
	const auto ImageFragment = GetImageFragmentFromInventoryItem(ClickedItem);
	if (ImageFragment == nullptr)
		return;

	if(!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInventoryHoverProxy>(GetOwningPlayer(), HoverItemClass);
	}

	const FVector2D DrawSize = GetDrawSize(*GridFragment);

	FSlateBrush IconBrush;
	// ToDo: async loading
	IconBrush.SetResourceObject(ImageFragment->GetIcon().LoadSynchronous());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize *UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
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

void UInventoryGrid::RemoveItemFromGrid(UInventoryItem* ClickedItem, const int32 GridIndex)
{
	const auto GridFragment = GetGridFragmentFromInventoryItem(ClickedItem);
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
}
#endif//WITH_EDITOR

