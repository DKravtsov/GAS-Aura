// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryGridWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Editor/WidgetCompilerLog.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Widgets/Inventory/GridSlot/InventoryGridSlotWidget.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverItemWidget.h"
#include "Widgets/Inventory/ItemPopUp/InventoryItemPopup.h"
#include "Widgets/Inventory/SlottedItems/InventorySlottedItemWidget.h"
#include "Widgets/Inventory/ViewModels//InventoryGridViewModel.h"

#include "DebugHelper.h"

void UInventoryGridWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());

	CreateGridViewModel();
	if (ensure(GridViewModel))
	{
		GridViewModel->OnItemAdded.AddUObject(this, &UInventoryGridWidget::AddItem);
		GridViewModel->OnStackChanged.AddUObject(this, &UInventoryGridWidget::OnStackChanged);
		//GridViewModel->OnItemRemovedFromGrid.AddUObject(this, &ThisClass::OnRemovedItemFromGrid);
		GridViewModel->GetOnGridSlotsResetDelegate().AddUObject(this, &UInventoryGridWidget::OnRemovedItemFromGrid);
		GridViewModel->GetOnGridSlotsUpdatedDelegate().AddUObject(this, &UInventoryGridWidget::OnUpdateGridSlots);
	}

	ConstructGrid();
}

void UInventoryGridWidget::CreateGridViewModel()
{
	if (!IsValid(GridViewModel))
	{
		GridViewModel = NewObject<UInventoryGridViewModel>(InventoryComponent->GetOwner());
		GridViewModel->Initialize(InventoryComponent.Get(), ItemCategory);
	}
}

void UInventoryGridWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
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

bool UInventoryGridWidget::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location)
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

void UInventoryGridWidget::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	if (!bMouseWithinCanvas)
		return;

	LastTileParameters = TileParameters;
	
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	TileParameters.TileCoordinates = HoveredTileCoordinates;
	TileParameters.TileIndex = UInventoryWidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);

	OnTileParametersUpdated(TileParameters);
}

void UInventoryGridWidget::OnTileParametersUpdated(const FInventoryTileParameters& Parameters)
{
	if (!IsValid(HoverItem))
		return;
	
	// Get Hover Item's dimensions
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	
	// calculate the starting coordinate for highlighting
	const FIntPoint StartCoords = CalculateStartingCoordinates(Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);
	ItemDropIndex = UInventoryWidgetUtils::GetIndexFromPosition(StartCoords, Columns);

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

FInventorySpaceQueryResult UInventoryGridWidget::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions) const
{
	FInventorySpaceQueryResult Result;

	// in the grid bounds?
	const int32 GridIndex = UInventoryWidgetUtils::GetIndexFromPosition(Position, Columns);
	if (!IsInGridBounds(GridIndex, Dimensions))
		return Result;

	Result.bHasSpace = true;
	
	// any items in the way?
	TSet<int32> OccupiedUpperLeftIndexes;
	UInventoryStatics::ForEach2D(GridSlots, GridIndex, Dimensions, Columns, [&](const UInventoryGridSlotWidget* GridSlot)
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

void UInventoryGridWidget::HighlightSlots(const int32 StartIndex, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas)
		return;

	UnHighlightSlots();
	
	UInventoryStatics::ForEach2D(GridSlots, StartIndex, Dimensions, Columns, [](UInventoryGridSlotWidget* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
		return true;
	});
	LastHighlightedIndex = StartIndex;
	LastHighlightedDimensions = Dimensions;
}

void UInventoryGridWidget::UnHighlightSlots(const int32 StartIndex, const FIntPoint& Dimensions)
{
	UInventoryStatics::ForEach2D(GridSlots, StartIndex, Dimensions, Columns, [](UInventoryGridSlotWidget* GridSlot)
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

void UInventoryGridWidget::ChangeHoverType(const int32 StartIndex, const FIntPoint& Dimensions, EInventoryGridSlotVisualState GridSlotState)
{
	UnHighlightSlots();
	UInventoryStatics::ForEach2D(GridSlots, StartIndex, Dimensions, Columns, [GridSlotState](UInventoryGridSlotWidget* GridSlot)->bool
	{
		GridSlot->SetGridSlotState(GridSlotState);
		return true;
	});
	LastHighlightedIndex = StartIndex;
	LastHighlightedDimensions = Dimensions;
}

FIntPoint UInventoryGridWidget::CalculateStartingCoordinates(const FIntPoint& Coordinate, const FIntPoint& Dimensions, EInventoryTileQuadrant Quadrant) const
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

FIntPoint UInventoryGridWidget::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// TODO: Make sure that TileSize is equal to actual Grid cell size
	
	const FIntPoint Coordinates{
		FMath::FloorToInt32((MousePosition.X - CanvasPosition.X) / TileSize),
		FMath::FloorToInt32((MousePosition.Y - CanvasPosition.Y) / TileSize)
	};
	return Coordinates;
}

EInventoryTileQuadrant UInventoryGridWidget::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
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

bool UInventoryGridWidget::IsInGridBounds(const int32 StartIndex, const FIntPoint& Dimensions) const
{
	if (!GridSlots.IsValidIndex(StartIndex))
		return false;
	const int32 EndColumn = StartIndex % Columns + Dimensions.X;
	const int32 EndRow = StartIndex / Columns + Dimensions.Y;

	return EndColumn <= Columns && EndRow <= Rows;
}

void UInventoryGridWidget::AddItem(const FInventorySlotAvailabilityResult& Result)
{
	UInventoryItem* Item = Result.Item.Get();
	if (!IsValid(Item) || !MatchesCategory(Item))
		return;
	
	LOG_NETFUNCTIONCALL_W_MSG(TEXT("Adding item: [%s]; count: %d"), GetInventoryItemId(Item), Result.TotalRoomToFill);

	AddItemToIndexes(Result, Item);
}

void UInventoryGridWidget::AddItemToIndexes(const FInventorySlotAvailabilityResult& Result, UInventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.Amount);
		// todo: needs to check
		//UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.Amount);
	}
}

void UInventoryGridWidget::AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable,	const int32 StackAmount)
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

UInventorySlottedItemWidget* UInventoryGridWidget::CreateSlottedItemWidget(UInventoryItem* Item, const int32 Index,
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
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &UInventoryGridWidget::OnSlottedItemClicked);
	
	return SlottedItem;
}

void UInventoryGridWidget::SetSlottedItemImage(const UInventorySlottedItemWidget* SlottedItem, const FInventoryItemGridFragment& GridFragment, const FInventoryItemImageFragment& ImageFragment) const
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

FSlateBrush UInventoryGridWidget::GetTempBrush()
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = FVector2D{1,1};
	return Brush;
}

void UInventoryGridWidget::PutHoverItemDown()
{
	if (!HasHoverItem())
		return;

	FInventorySlotAvailabilityResult Result;
	ensure(InventoryComponent->GetInventoryStorage()->HasRoomForItem(Result, HoverItem->GetInventoryItem()->GetItemManifest(), HoverItem->GetStackCount()));
	
	Result.Item = HoverItem->GetInventoryItem();

	GridViewModel->NotifyStackChanged(Result);

	ClearHoverItem();
}

void UInventoryGridWidget::AddSlottedItemToGrid(const int32 Index, const FInventoryItemGridFragment& GridFragment, UInventorySlottedItemWidget* SlottedItem) const
{
	const FIntPoint Pos = UInventoryWidgetUtils::GetPositionFromIndex(Index, Columns);
	if (UGridSlot* const GridSlot = GridWidget->AddChildToGrid(SlottedItem, Pos.Y, Pos.X))
	{
		GridSlot->SetColumnSpan(GridFragment.GetGridSize().X);
		GridSlot->SetRowSpan(GridFragment.GetGridSize().Y);
	}
}

void UInventoryGridWidget::OnUpdateGridSlots(const TArray<int32>& GridIndexArray)
{
	for (const int32 GridIndex : GridIndexArray)
	{
		if (ensure(GridSlots.IsValidIndex(GridIndex)))
		{
			GridSlots[GridIndex]->SetOccupiedTexture();
		}
	}
}

void UInventoryGridWidget::ConstructGrid()
{
	check(GridViewModel);
	Rows = GridViewModel->GetRows();
	Columns = GridViewModel->GetColumns();
	
	check(Rows > 0 && Columns > 0);
	
	GridSlots.Reserve(Rows * Columns);

	GridWidget->ClearChildren();

	for (int RowIndex = 0; RowIndex < Rows; RowIndex++)
	{
		for (int ColumnIndex = 0; ColumnIndex < Columns; ColumnIndex++)
		{
			UInventoryGridSlotWidget* InventoryGridSlot = CreateWidget<UInventoryGridSlotWidget>(this, GridSlotClass);
			
			GridWidget->AddChildToGrid(InventoryGridSlot, RowIndex, ColumnIndex);
			
			const int32 AddedIndex = GridSlots.Add(InventoryGridSlot);
			check(AddedIndex == UInventoryWidgetUtils::GetIndexFromPosition(FIntPoint(ColumnIndex, RowIndex), Columns));
			InventoryGridSlot->Bind(GridViewModel->GetStorageGrid().Get(), AddedIndex);

			InventoryGridSlot->OnGridSlotClicked.AddDynamic(this, &UInventoryGridWidget::OnGridSlotClicked);
			InventoryGridSlot->OnGridSlotHovered.AddDynamic(this, &UInventoryGridWidget::OnGridSlotHovered);
			InventoryGridSlot->OnGridSlotUnhovered.AddDynamic(this, &UInventoryGridWidget::OnGridSlotUnhovered);
		}
	}
}

bool UInventoryGridWidget::MatchesCategory(const UInventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory().MatchesTag(ItemCategory);
}

void UInventoryGridWidget::OnStackChanged(const FInventorySlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get()))
		return;

	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlotWidget = GridSlots[Availability.Index];
			const int32 NewStackCount = GridSlotWidget->GetStackCount();
			const auto& SlottedItemWidget = SlottedItems.FindChecked(Availability.Index);
			SlottedItemWidget->UpdateStackCount(NewStackCount);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.Amount);
			//UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.Amount);
		}
	}
}

bool UInventoryGridWidget::IsLeftMouseButtonClick(const FPointerEvent& MouseEvent)
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

bool UInventoryGridWidget::IsRightMouseButtonClick(const FPointerEvent& MouseEvent)
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

void UInventoryGridWidget::PickUpItemInInventory(UInventoryItem* ClickedItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedItem, GridIndex, GridIndex);

	RemoveItemFromGrid(ClickedItem, GridIndex);
}

void UInventoryGridWidget::PutDownItemInInventoryAtIndex(const int32 GridIndex)
{
	check(IsValid(HoverItem));
	AddItemAtIndex(HoverItem->GetInventoryItem(), GridIndex, HoverItem->IsStackable(), HoverItem->GetStackCount());
	GridViewModel->UpdateGridSlots(HoverItem->GetInventoryItem(), GridIndex, HoverItem->IsStackable(), HoverItem->GetStackCount());

	ClearHoverItem();
}

void UInventoryGridWidget::ShowDefaultCursor() const
{
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, nullptr);
}

void UInventoryGridWidget::ClearHoverItem()
{
	if (!IsValid(HoverItem))
		return;

	HoverItem->Reset();
	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	ShowDefaultCursor();
}

void UInventoryGridWidget::SwapWithHoverItem(UInventoryItem* ClickedInventoryItem, const int32 GridIndex)
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
	GridViewModel->UpdateGridSlots(TempItem, ItemDropIndex, bTempStackable, TempStackCount);
}

void UInventoryGridWidget::AssignHoverItem(UInventoryItem* ClickedItem, const int32 GridIndex, const int32 PrevGridIndex)
{
	const auto GridFragment = UInventoryWidgetUtils::GetGridFragmentFromInventoryItem(ClickedItem);
	if (GridFragment == nullptr)
		return;
	const auto ImageFragment = UInventoryWidgetUtils::GetImageFragmentFromInventoryItem(ClickedItem);
	if (ImageFragment == nullptr)
		return;

	if(!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInventoryHoverItemWidget>(GetOwningPlayer(), HoverItemClass);
	}

	const FVector2D DrawSize = UInventoryWidgetUtils::GetDrawSize(*GridFragment, TileSize) * UWidgetLayoutLibrary::GetViewportScale(this);;

	ImageFragment->GetIcon().LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateLambda([DrawSize, this](const FSoftObjectPath& SoftPath, UObject* LoadedObject)
	{
		if (!IsValid(LoadedObject))
		{
			UE_LOG(LogInventory, Error, TEXT("Loading failed: [%s]"), *SoftPath.ToString());
			return;
		}
		if (!IsValid(HoverItem))
			return; // it's too late, the hover item is destroyed. It happens when we drop item using Drop button in the menu
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

void UInventoryGridWidget::OnHide()
{
	PutHoverItemDown();
	ShowDefaultCursor();
}

void UInventoryGridWidget::RemoveItemFromGrid(UInventoryItem* Item)
{
	check(IsValid(Item));
	check(!Item->IsStackable()); // this method must not be called for stackable items

	for (const auto& [GridIndex, SlottedItem] : SlottedItems)
	{
		if (IsValid(SlottedItem))
		{
			if (SlottedItem->GetInventoryItem() == Item)
			{
				RemoveItemFromGrid(Item, GridIndex);
				break;
			}
		}
	}
}

void UInventoryGridWidget::RemoveItemFromGrid(UInventoryItem* ClickedItem, const int32 GridIndex)
{
	GridViewModel->RemoveItemFromGrid(ClickedItem, GridIndex);
}

void UInventoryGridWidget::OnRemovedItemFromGrid(UInventoryItem* ItemToRemove, int32 GridIndex)
{
	FIntPoint Dimensions = UInventoryWidgetUtils::GetGridDimensionsOfItem(ItemToRemove);
	UInventoryStatics::ForEach2D(GridSlots, GridIndex, Dimensions, Columns, [](UInventoryGridSlotWidget* GridSlot)
	{
		GridSlot->SetDefaultTexture();
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

void UInventoryGridWidget::OnRemovedItemFromGrid(const TArray<int32>& GridIndexArray)
{
	for (const int32 GridIndex : GridIndexArray)
	{
		if (!ensure(GridSlots.IsValidIndex(GridIndex)))
			continue;
		GridSlots[GridIndex]->SetDefaultTexture();

		if (SlottedItems.Contains(GridIndex))
		{
			TObjectPtr<UInventorySlottedItemWidget> FoundSlottedItem;
			if (SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem))
			{
				FoundSlottedItem->RemoveFromParent();
			}
		}
	}
}

void UInventoryGridWidget::OnGridSlotClicked(int32 GridSlotIndex, const FPointerEvent& MouseEvent)
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

void UInventoryGridWidget::OnGridSlotHovered(int32 GridSlotIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem))
		return;

	const auto GridSlot = GridViewModel->GetGridSlot(GridSlotIndex);
	if (GridSlot.IsAvailable())
	{
		const auto GridSlotWidget = GridSlots[GridSlotIndex];
		check(GridSlotWidget);
		GridSlotWidget->SetOccupiedTexture();
	}
}

void UInventoryGridWidget::OnGridSlotUnhovered(int32 GridSlotIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem))
		return;

	if (GridViewModel->IsGridSlotAvailable(GridSlotIndex))
	{
		const auto GridSlotWidget = GridSlots[GridSlotIndex];
		check(GridSlotWidget);
		GridSlotWidget->SetDefaultTexture();
	}
}

bool UInventoryGridWidget::IsHoverItemSameStackableAs(const UInventoryItem* ClickedInventoryItem) const
{
	const bool bSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemType());
}

int32 UInventoryGridWidget::GetMaxStackSize(const UInventoryItem* Item)
{
	check(IsValid(Item));
	if (const auto StackableFragment = Item->GetItemManifest().GetFragmentOfType<FInventoryItemStackableFragment>())
	{
		return StackableFragment->GetMaxStackSize();
	}
	return 1;
}

void UInventoryGridWidget::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
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

void UInventoryGridWidget::CreateItemPopupMenu(const int32 GridIndex)
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

void UInventoryGridWidget::SwapStackCountsWithHoverItem(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 GridIndex)
{
	UpdateStackCountInSlot(GridIndex, HoveredStackCount);
	HoverItem->UpdateStackCount(ClickedStackCount);
}

void UInventoryGridWidget::FillInStacksOrConsumeHover(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 MaxStackCount, const int32 GridIndex)
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

void UInventoryGridWidget::UpdateStackCountInSlot(const int32 GridIndex, const int32 NewStackCount)
{
	GridViewModel->UpdateStackCount(GridIndex, NewStackCount);
	
	UInventorySlottedItemWidget* SlottedItem = SlottedItems.FindChecked(GridIndex);
	SlottedItem->UpdateStackCount(NewStackCount);
}

void UInventoryGridWidget::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}

void UInventoryGridWidget::OnPopupMenuSplit(const int32 SplitAmount, const int32 GridIndex)
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

void UInventoryGridWidget::OnPopupMenuConsume(const int32 GridIndex)
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

void UInventoryGridWidget::OnPopupMenuDrop(const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if( !IsValid(RightClickedItem))
		return;
	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetStartIndex();
	PickUpItemInInventory(RightClickedItem, UpperLeftIndex);
	DropHoverItemOnGround();
}

void UInventoryGridWidget::DropHoverItemOnGround()
{
	if (!IsValid(HoverItem) || !IsValid(HoverItem->GetInventoryItem()))
		return;

	InventoryComponent->DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());

	ClearHoverItem();
	ShowDefaultCursor();
}

bool UInventoryGridWidget::HasHoverItem() const
{
	return IsValid(HoverItem);
}

#if WITH_EDITOR
void UInventoryGridWidget::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

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
