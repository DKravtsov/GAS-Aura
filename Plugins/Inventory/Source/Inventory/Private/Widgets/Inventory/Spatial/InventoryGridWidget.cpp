// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryGridWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Store/Components/InventoryStoreComponent.h"
#include "Items/InventoryItem.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Widgets/Inventory/GridSlot/InventoryGridSlotWidget.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverItemWidget.h"
#include "Widgets/Inventory/ItemPopUp/InventoryItemPopup.h"
#include "Widgets/Inventory/SlottedItems/InventorySlottedItemWidget.h"
#include "Widgets/Inventory/ViewModels//InventoryGridViewModel.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif
#include "DebugHelper.h"
#include "InventoryManagement/Storage/InventoryStorage.h"

void UInventoryGridWidget::NativeOnInitialized()
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Initializing Grid Widget"));
	
	Super::NativeOnInitialized();

	InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());

	if (bIsStoreGrid)
		return;
	
	CreateGridViewModel(InventoryComponent->GetInventoryStorage());

	ConstructGrid();
}

void UInventoryGridWidget::ConstructForStore(UInventoryStoreComponent* Store)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Initializing Grid Widget for Store"));
	
	CreateGridViewModel(Store->GetInventoryStorage());

	ConstructGrid();
}

void UInventoryGridWidget::CreateGridViewModel(UInventoryStorage* InventoryStorage)
{
	if (!IsValid(GridViewModel))
	{
		LOG_NETFUNCTIONCALL
		
		GridViewModel = NewObject<UInventoryGridViewModel>(GetOwningPlayer());
		GridViewModel->Initialize(GetOwningPlayer(), InventoryStorage, ItemCategory);

		GridViewModel->GetOnItemAddedToGridDelegate().AddUObject(this, &UInventoryGridWidget::HandleAddItemToGrid);
		GridViewModel->GetOnStackChangedDelegate().AddUObject(this, &UInventoryGridWidget::HandleOnStackChanged);
		GridViewModel->GetOnGridSlotsResetDelegate().AddUObject(this, &UInventoryGridWidget::HandleOnRemovedItemFromGrid);
		GridViewModel->GetOnGridSlotsUpdatedDelegate().AddUObject(this, &UInventoryGridWidget::HandleOnUpdateGridSlots);
		
		InventoryComponent->OnSellItemResult.AddUObject(this, &UInventoryGridWidget::HandleSellItemResult);
		InventoryComponent->OnBuyItemResult.AddUObject(this, &UInventoryGridWidget::HandleBuyItemResult);
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
	if (!HasHoverItem())
		return;
	
	// Get Hover Item's dimensions
	const FIntPoint Dimensions = GetHoverItem()->GetGridDimensions();
	
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

void UInventoryGridWidget::HandleAddItemToGrid(const FInventorySlotAvailabilityResult& Result)
{
	UInventoryItem* Item = Result.Item.Get();
	if (!IsValid(Item) || !MatchesCategory(Item))
		return;
	
	LOG_NETFUNCTIONCALL_MSG(TEXT("Adding item: [%s]; count: %d"), *GetInventoryItemId(Item), Result.TotalRoomToFill);

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

void UInventoryGridWidget::AddItemAtIndex(UInventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
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

void UInventoryGridWidget::UpdateInventoryGridSlots()
{
	for (const auto& GridSlot : GridSlots)
	{
		if (GridSlot->IsAvailable())
			continue;
		if (SlottedItems.Contains(GridSlot->GetStartIndex()))
			continue;
		AddItemAtIndex(GridSlot->GetInventoryItem().Get(), GridSlot->GetStartIndex(), GridSlot->GetInventoryItem()->IsStackable(), GridSlot->GetStackCount());
	}
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
	SlottedItem->SetImageBrush(UInventoryWidgetUtils::GetTempBrush());
	
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

void UInventoryGridWidget::PutHoverItemDown()
{
	if (!HasHoverItem())
		return;

	InventoryComponent->Server_PutSelectedItemToStorage();
}

void UInventoryGridWidget::AddSlottedItemToGrid(const int32 Index, const FInventoryItemGridFragment& GridFragment, UInventorySlottedItemWidget* SlottedItem) const
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Adding slotted item: [%s]; index: %d"), *GetInventoryItemId(SlottedItem->GetInventoryItem()), Index);

	const FIntPoint Pos = UInventoryWidgetUtils::GetPositionFromIndex(Index, Columns);
	if (UGridSlot* const GridSlot = GridWidget->AddChildToGrid(SlottedItem, Pos.Y, Pos.X))
	{
		GridSlot->SetColumnSpan(GridFragment.GetGridSize().X);
		GridSlot->SetRowSpan(GridFragment.GetGridSize().Y);
	}
}

UInventorySlottedItemWidget* UInventoryGridWidget::GetSlottedItemWidgetAtIndex(const int32 UpperLeftIndex) const
{
	const TObjectPtr<UInventorySlottedItemWidget>* SlottedItemPtr = SlottedItems.Find(UpperLeftIndex);
	return SlottedItemPtr ? SlottedItemPtr->Get() : nullptr;
}

void UInventoryGridWidget::HandleOnUpdateGridSlots(const TArrayView<int32>& GridIndexArray)
{
	LOG_NETFUNCTIONCALL

	const bool bHasAuthority = GetOwningPlayer()->HasAuthority();
	
	for (const int32 GridIndex : GridIndexArray)
	{
		check(GridSlots.IsValidIndex(GridIndex));
		UInventoryGridSlotWidget* GridSlot = GridSlots[GridIndex];
		if (!GridSlot->IsAvailable())
		{
			//if (!bHasAuthority)
			{
				int32 UpperLeftIndex = GridSlot->GetStartIndex();
				UInventorySlottedItemWidget* SlottedItem = GetSlottedItemWidgetAtIndex(UpperLeftIndex);
				if (!SlottedItem || (!bHasAuthority && SlottedItem->GetInventoryItem() != GridSlot->GetInventoryItem().Get()))
				{
					if (SlottedItem)
					{
						TObjectPtr<UInventorySlottedItemWidget> FoundSlottedItem;
						if (SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem))
						{
							FoundSlottedItem->RemoveFromParent();
						}
					}

					UInventoryItem* NewItem = GridSlot->GetInventoryItem().Get();
					const bool bStackable = NewItem->IsStackable();
					const int32 StackSize = GridSlot->GetStackCount();
					AddItemAtIndex(NewItem, UpperLeftIndex, bStackable, StackSize);
				}
				else if (GridSlot->GetTileIndex() == GridSlot->GetStartIndex())
				{
					SlottedItem->UpdateStackCount(GridSlot->GetStackCount());
				}
			}
			GridSlot->SetOccupiedTexture();
		}
		else
		{
			GridSlot->SetDefaultTexture();
		}
	}
}

void UInventoryGridWidget::ConstructGrid()
{
	LOG_NETFUNCTIONCALL
	
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

void UInventoryGridWidget::HandleOnStackChanged(const FInventorySlotAvailabilityResult& Result)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s] New total stack amount: %d"), *GetInventoryItemId(Result.Item.Get()), Result.TotalRoomToFill)
	
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
	LOG_NETFUNCTIONCALL_MSG(TEXT("Index: %d"), GridIndex)
	
	InventoryComponent->Server_SelectItem(ClickedItem, GridIndex, GridIndex);
}

void UInventoryGridWidget::PutDownItemInInventoryAtIndex(const int32 GridIndex)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("Index: %d"), GridIndex)
	
	if(!HasHoverItem())
	{
		UE_LOG(LogInventory, Error, TEXT("Hover item is not valid"));
		return;
	}

	auto* HoverItem = GetHoverItem();
	if (GridViewModel->IsMyItem(HoverItem->GetInventoryItem()))
	{
		InventoryComponent->Server_PutSelectedItemToStorageAtIndex(GridIndex);
	}
	else
	{
		// we're trying to sell or to buy the item by drag-n-drop

		// item must have the same category
		check(IsValid(HoverItem->GetInventoryItem()) && HoverItem->GetInventoryItem()->GetItemManifest().GetItemCategory() == ItemCategory);

		if (IsStoreGrid())
		{
			// Selling

			InventoryComponent->Server_SellItem(HoverItem->GetInventoryItem(), HoverItem->GetPreviousGridIndex(), HoverItem->GetStackCount());
		}
		else
		{
			// Purchasing

			InventoryComponent->Server_BuyItem(HoverItem->GetInventoryItem(), HoverItem->GetPreviousGridIndex(), HoverItem->GetStackCount());
		}
	}
}

void UInventoryGridWidget::SwapWithHoverItem(UInventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	LOG_NETFUNCTIONCALL
	
	if (!HasHoverItem())
	{
		UE_LOG(LogInventory, Error, TEXT("Hover item is not valid"));
		return;
	}

	// UInventoryItem* TempItem = HoverItem->GetInventoryItem();
	// const int32 TempStackCount = HoverItem->GetStackCount();
	// const bool bTempStackable = HoverItem->IsStackable();
	//
	// AddItemAtIndex(TempItem, ItemDropIndex, bTempStackable, TempStackCount);
	
	InventoryComponent->Server_SwapSelectedWitItem(ClickedInventoryItem, GridIndex);
}

void UInventoryGridWidget::OnHide()
{
	PutHoverItemDown();
}

void UInventoryGridWidget::HandleOnRemovedItemFromGrid(const TArrayView<int32>& GridIndexArray)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT(" (num=%d)"), GridIndexArray.Num())
	
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
	auto* HoverItem = GetHoverItem();
	LOG_NETFUNCTIONCALL_MSG(TEXT("GridIndex [%d]; HoverItem [%s]"), GridSlotIndex,
		HoverItem ? *GetInventoryItemId(HoverItem->GetInventoryItem()) : TEXT("None"));

	if (!IsValid(HoverItem) || !GridSlots.IsValidIndex(ItemDropIndex))
		return;

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	if (!IsInGridBounds(ItemDropIndex, HoverItem->GetGridDimensions()))
		return;

	const auto& GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownItemInInventoryAtIndex(ItemDropIndex);
	}
}

void UInventoryGridWidget::OnGridSlotHovered(int32 GridSlotIndex, const FPointerEvent& MouseEvent)
{
	if (HasHoverItem())
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
	if (HasHoverItem())
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
	auto* HoverItem = GetHoverItem();
	check(IsValid(HoverItem));
	const bool bSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bSameItem && bIsStackable; //&& HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemType());
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
	UInventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	LOG_NETFUNCTIONCALL_MSG(TEXT("GridIndex [%d]; ClickedItem [%s]; HoverItem [%s]"), GridIndex,
		*GetInventoryItemId(ClickedInventoryItem),
		GetHoverItem() ? *GetInventoryItemId(GetHoverItem()->GetInventoryItem()) : TEXT("None"));

	// remove the item description widget
	UInventoryStatics::ItemUnhovered(GetOwningPlayer());

	if (!HasHoverItem())
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
		const int32 HoveredStackCount = GetHoverItem()->GetStackCount();
		const int32 MaxStackSize = GetMaxStackSize(ClickedInventoryItem);
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		
		// Should we swap their stack counts?
		if (ClickedStackCount == MaxStackSize && HoveredStackCount < MaxStackSize)
		{
			InventoryComponent->Server_SwapStackCountWithHoverItem(ClickedInventoryItem, GridIndex);
//			SwapStackCountsWithHoverItem(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}

		InventoryComponent->Server_FillInStacksOrConsumeHover(ClickedInventoryItem, GridIndex);
//		FillInStacksOrConsumeHover(ClickedStackCount, HoveredStackCount, MaxStackSize, GridIndex);
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

		if (IsStoreGrid())
		{
			ItemPopupMenu->CollapseDropButton();
		}

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

		if (RightClickedItem->IsConsumable() && !IsStoreGrid())
		{
			ItemPopupMenu->OnConsumeDelegate.BindUObject(this, &ThisClass::OnPopupMenuConsume);
		}
		else
		{
			ItemPopupMenu->CollapseConsumeButton();
		}
		if (InventoryComponent->IsStoreMenuOpen())
		{
			if (IsStoreGrid())
			{
				ItemPopupMenu->CollapseSellButton();
				ItemPopupMenu->OnBuyItemDelegate.BindUObject(this, &ThisClass::OnPopupMenuBuy);
			}
			else
			{
				ItemPopupMenu->CollapseBuyButton();
				ItemPopupMenu->OnSellItemDelegate.BindUObject(this, &ThisClass::OnPopupMenuSell);
			}
		}
		else
		{
			ItemPopupMenu->CollapseSellButton();
			ItemPopupMenu->CollapseBuyButton();
		}
	}
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

	InventoryComponent->Server_SplitStackToHoverItem(RightClickedItem, UpperLeftIndex, SplitAmount);
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
	
	InventoryComponent->ConsumeItem(RightClickedItem, GridIndex, 1);
}

void UInventoryGridWidget::OnPopupMenuDrop(const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if( !IsValid(RightClickedItem))
		return;
	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetStartIndex();
	const int32 StackCount = GridSlots[UpperLeftIndex]->GetStackCount();

	InventoryComponent->Server_DropItem(RightClickedItem, UpperLeftIndex, StackCount);
}

void UInventoryGridWidget::OnPopupMenuBuy(const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if( !IsValid(RightClickedItem))
		return;
	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetStartIndex();

	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s] index %d"), *GetInventoryItemId(RightClickedItem), UpperLeftIndex)

	checkf(InventoryComponent.IsValid() && IsValid(InventoryComponent->GetOpenedStore()), TEXT("The Store should be opened first"))
	
	// Do some pre-checks to not call RPC if it fails anyway
	
	const int32 StackCount = GridSlots[UpperLeftIndex]->GetStackCount();
	FInventorySlotAvailabilityResult Result;
	if (!InventoryComponent->GetInventoryStorage()->HasRoomForItem(Result, RightClickedItem->GetItemManifest(), StackCount))
	{
		UE_LOG(LogInventory, Error, TEXT("There's not enough room in for the item [%s]"), *GetInventoryItemId(RightClickedItem))
		return;
	}

	InventoryComponent->Server_BuyItem(RightClickedItem, UpperLeftIndex, GridSlots[UpperLeftIndex]->GetStackCount());
}

void UInventoryGridWidget::OnPopupMenuSell(const int32 GridIndex)
{
	check(GridSlots.IsValidIndex(GridIndex));
	check(InventoryComponent.IsValid());
	
	UInventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem))
		return;
	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetStartIndex();

	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s] index %d"), *GetInventoryItemId(RightClickedItem), UpperLeftIndex)

	// Do some pre-checks to not call RPC if it fails anyway
	
	if (const auto* Store = InventoryComponent->GetOpenedStore())
	{
		const int32 StackCount = GridSlots[UpperLeftIndex]->GetStackCount();
		FInventorySlotAvailabilityResult Result;
		if (!Store->GetInventoryStorage()->HasRoomForItem(Result, RightClickedItem->GetItemManifest(), StackCount))
		{
			UE_LOG(LogInventory, Error, TEXT("There's not enough room in the store for the item [%s]"), *GetInventoryItemId(RightClickedItem))
			return;
		}
	}
	else
	{
		UE_LOG(LogInventory, Error, TEXT("Store is null"))
		return;
	}

	PickUpItemInInventory(RightClickedItem, UpperLeftIndex);
	InventoryComponent->Server_SellSelectedItem();
}

void UInventoryGridWidget::DropHoverItemOnGround()
{
	if (HasHoverItem())
		InventoryComponent->Server_DropSelectedItemOff();
}

bool UInventoryGridWidget::HasHoverItem() const
{
	return InventoryComponent->HasItemSelected();
}

UInventoryHoverItemWidget* UInventoryGridWidget::GetHoverItem() const
{
	return UInventoryStatics::GetHoverItem(GetOwningPlayer());
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
	if (!ItemPopupMenuClass)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(" has no ItemPopupMenuClass specified.")));
	}
}
#endif//WITH_EDITOR

void UInventoryGridWidget::HandleSellItemResult(bool bSuccess, const FString& ErrorMessage)
{
	UE_LOG(LogInventory, Log, TEXT("SellItemResult: %s"), *ErrorMessage);

	if (bSuccess)
	{
		// Show an in-game message?
	}
}

void UInventoryGridWidget::HandleBuyItemResult(bool bSuccess, const FString& ErrorMessage)
{
	UE_LOG(LogInventory, Log, TEXT("BuyItemResult: %s"), *ErrorMessage);

	if (bSuccess)
	{
		// show an in-game message?
	}
}

