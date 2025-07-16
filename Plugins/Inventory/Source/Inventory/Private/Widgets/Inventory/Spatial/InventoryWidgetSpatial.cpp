// Copyright 4sandwiches


#include "Widgets/Inventory/Spatial/InventoryWidgetSpatial.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Editor/WidgetCompilerLog.h"
#include "EquipmentManagement/Components/InventoryEquipmentComponent.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/InventoryStatics.h"
#include "Items/InventoryItem.h"
#include "Widgets/Inventory/GridSlot/InventoryEquippedGridSlot.h"
#include "Widgets/Inventory/HoverProxy/InventoryHoverItemWidget.h"
#include "Widgets/Inventory/SlottedItems/InventoryEquippedSlottedItemWidget.h"
#include "Widgets/Inventory/Spatial/InventoryGridWidget.h"
#include "Widgets/ItemDescription/InventoryItemDescription.h"

#include "DebugHelper.h"

void UInventoryWidgetSpatial::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equipment->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowEquipmentGrid);
	Button_Consumable->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowConsumableGrid);
	Button_Crafting->OnClicked.AddDynamic(this, &UInventoryWidgetSpatial::ShowCraftingGrid);

	InventoryGrid_Equipment->SetOwningCanvas(CanvasPanel);
	InventoryGrid_Consumable->SetOwningCanvas(CanvasPanel);
	InventoryGrid_Crafting->SetOwningCanvas(CanvasPanel);
	
	ShowEquipmentGrid();

	UInventoryComponent* InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(InventoryComponent);
	WidgetTree->ForEachWidget([this, InventoryComponent](UWidget* Widget)
	{
		if (auto EquipmentSlotWidget = Cast<UInventoryEquippedGridSlot>(Widget))
		{
			if (EquipmentSlotWidget->Bind(InventoryComponent, EquipmentSlotWidget->GetSlotId()))
			{
				EquippedGridSlots.Emplace(EquipmentSlotWidget);
			    EquipmentSlotWidget->EquippedGridSlotClicked.AddDynamic(this, &UInventoryWidgetSpatial::EquippedGridSlotClicked);
			}
			else
			{
				UE_LOG(LogInventory, Warning, TEXT("Equipment slot not found: %s"),
					*UEnum::GetValueAsString(EquipmentSlotWidget->GetSlotId()));
			}
		}
	});

	InventoryComponent->OnItemEquipped.AddDynamic(this, &UInventoryWidgetSpatial::UpdateEquippedItemStatus);
	InventoryComponent->OnItemUnequipped.AddDynamic(this, &UInventoryWidgetSpatial::UpdateEquippedItemStatus);
}

FReply UInventoryWidgetSpatial::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->DropHoverItemOnGround();
	}
	return FReply::Handled();
}

void UInventoryWidgetSpatial::OnInventoryHovered(UInventoryItem* Item)
{
	auto ItemDescWidget = GetOrCreateItemDescription();
	ItemDescWidget->Hide();
	auto EquippedItemDescWidget = GetOrCreateEquippedItemDescription();
	EquippedItemDescWidget->Hide();
	SetToolTip(nullptr);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(TimerHandle_EquippedDescription);

	const auto& Manifest = Item->GetItemManifest();
	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, &Manifest, ItemDescWidget, Item]()
	{
		ItemDescWidget->Show();
		SetToolTip(ItemDescWidget);

		Manifest.AssimilateInventoryFragments(ItemDescWidget);

		// for the second item description, showing the equipped item of the same type
		const FTimerDelegate EquippedTimerDelegate = FTimerDelegate::CreateUObject(this, &UInventoryWidgetSpatial::ShowEquippedItemDescription, Item);
		GetOwningPlayer()->GetWorldTimerManager().SetTimer(TimerHandle_EquippedDescription, EquippedTimerDelegate, EquippedDescriptionDelay, false);
	});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(TimerHandle_Description, TimerDelegate, DescriptionDelay, false);
}

void UInventoryWidgetSpatial::ShowEquippedItemDescription(UInventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	const auto* EquipFragment = Manifest.GetFragmentOfType<FInventoryItemEquipmentFragment>();
	if (!EquipFragment)
		return;

	const FGameplayTag HoveredEquipmentTypeTag = EquipFragment->GetEquipmentType();

	if (FindSlotWithEquippedItem(Item))
		return; // it's already equipped

	// Find the equipped item of the same type
	const auto* EquippedGridSlotPtr = EquippedGridSlots.FindByPredicate([HoveredEquipmentTypeTag](const UInventoryEquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem().IsValid() &&
			GridSlot->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FInventoryItemEquipmentFragment>()->GetEquipmentType().MatchesTagExact(HoveredEquipmentTypeTag);
	});
	const auto FoundEquippedSlot = EquippedGridSlotPtr ? EquippedGridSlotPtr->Get() : nullptr;
	if (!IsValid(FoundEquippedSlot))
		return;

	if (!FoundEquippedSlot->GetInventoryItem().IsValid())
		return;

	UInventoryItem* EquippedItem = FoundEquippedSlot->GetInventoryItem().Get();
	auto DescriptionWidget = GetOrCreateItemDescription();
	auto EquippedDescriptionWidget = GetOrCreateEquippedItemDescription();
	DescriptionWidget->InsertEquippedDescription(EquippedDescriptionWidget);
	EquippedDescriptionWidget->Show();

	EquippedItem->GetItemManifest().AssimilateInventoryFragments(EquippedDescriptionWidget);
}

void UInventoryWidgetSpatial::OnInventoryUnhovered()
{
	GetOrCreateItemDescription()->Hide();
	GetOrCreateEquippedItemDescription()->Collapse();
	SetToolTip(nullptr);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(TimerHandle_Description);	
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(TimerHandle_EquippedDescription);	
}

bool UInventoryWidgetSpatial::HasHoverItem() const
{
	return ActiveGrid->HasHoverItem();
}

UInventoryHoverItemWidget* UInventoryWidgetSpatial::GetHoverItem() const
{
	return ActiveGrid.IsValid() ? ActiveGrid->GetHoverItem() : nullptr;
}

float UInventoryWidgetSpatial::GetTileSize() const
{
	return ActiveGrid.IsValid() ? ActiveGrid->GetTileSize() : 0.f;
}

void UInventoryWidgetSpatial::OnCloseMenu()
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->OnHide();
	}
}

void UInventoryWidgetSpatial::OnOpenedMenu()
{
	FTimerHandle TimerHandle;
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		for (auto& EquippedGridSlot : EquippedGridSlots)
		{
			EquippedGridSlot->UpdateIfPending();
		}
	}, 0.1f, false);
}

void UInventoryWidgetSpatial::ShowEquipmentGrid()
{
	SetActiveGrid(InventoryGrid_Equipment, Button_Equipment);
}

void UInventoryWidgetSpatial::ShowConsumableGrid()
{
	SetActiveGrid(InventoryGrid_Consumable, Button_Consumable);
}

void UInventoryWidgetSpatial::ShowCraftingGrid()
{
	SetActiveGrid(InventoryGrid_Crafting, Button_Crafting);
}

// bool UInventoryWidgetSpatial::TryEquipItem(UInventoryItem* ItemToEquip, const FGameplayTag& EquipmentTypeTag, bool bAlwaysEquip, UInventoryItem** PreviousEquippedItem)
// {
// 	check(IsValid(ItemToEquip));
// 	const FGameplayTag EquipmentType = EquipmentTypeTag.IsValid() ? EquipmentTypeTag :  FindItemBestEquipType(ItemToEquip);
// 	if (!EquipmentType.IsValid())
// 		return false;
// 	
// 	UInventoryEquippedGridSlot* GridSlot = FindEquippedGridSlotByType(EquipmentType);
// 	if (!IsValid(GridSlot))
// 		return false;
// 	
// 	UInventoryItem* ItemToUnequip = nullptr;
// 	if (!GridSlot->IsAvailable())
// 	{
// 		if (bAlwaysEquip)
// 		{
// 			ItemToUnequip = GridSlot->GetInventoryItem().Get();
// 		}
// 		else
// 		{
// 			return false;
// 		}
// 	}
// 	
// 	const float TileSize = GetTileSize();
// 	if (UInventoryEquippedSlottedItemWidget* EquippedSlottedItem = GridSlot->OnItemEquipped(ItemToEquip, EquipmentType, TileSize))
// 	{
// 		EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &UInventoryWidgetSpatial::EquippedSlottedItemClicked);
//
// 		if (!GetOwningPlayer()->HasAuthority())
// 		{
// 			// Inform the server that we've equipped an item (potentially unequipping an item as well)
// 			UInventoryComponent* InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
// 			check(IsValid(InventoryComponent));
// 		
// 			InventoryComponent->EquipItem(ItemToEquip, ItemToUnequip);
// 		}
//
// 	}
// 	InventoryGrid_Equipment->RemoveItemFromGrid(ItemToEquip);
//
// 	return true;
// }

void UInventoryWidgetSpatial::UpdateEquippedItemStatus(UInventoryItem* Item)
{
	if (Item == nullptr)
		return;

	LOG_NETFUNCTIONCALL_MSG(TEXT("Item [%s]"), *GetInventoryItemId(Item));
	
	UInventoryComponent* InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(InventoryComponent != nullptr);

	const FInventoryEquipmentSlot* EquipmentSlot = InventoryComponent->FindEquipmentSlotByEquippedItem(Item);

	// Because EquippedGridSlot is bound to Equipment slot, if there is no EquipmentSlot containing the item, there also will be no EquippedGridSlot containing it.
	// That's why we need to search also in EquippedSlottedItem related to this slot widget.
	static auto FindSlotWidget = [](const UInventoryWidgetSpatial* W, const UInventoryItem* TestItem)-> UInventoryEquippedGridSlot*
	{
		const auto FoundSlotPtr = W->EquippedGridSlots.FindByPredicate([TestItem](const UInventoryEquippedGridSlot* EquippedGridSlot)
        	{
        		return EquippedGridSlot->GetInventoryItem() == TestItem ||
        			(EquippedGridSlot->GetEquippedSlottedItem() && EquippedGridSlot->GetEquippedSlottedItem()->GetInventoryItem() == TestItem);
        	});
		return FoundSlotPtr ? FoundSlotPtr->Get() : nullptr;
	};
	
	UInventoryEquippedGridSlot* EquippedItemSlotWidget = FindSlotWidget(this, Item);
	const bool bItemEquipped = EquipmentSlot != nullptr;
	const bool bItemShownAsEquipped = EquippedItemSlotWidget != nullptr && EquippedItemSlotWidget->GetEquippedSlottedItem() != nullptr;
	if (bItemEquipped == bItemShownAsEquipped)
		return;

	if (bItemEquipped)
	{
		// UE_LOG(LogTemp, Warning, TEXT("EquipDeSync: Item [%s] is equipped in slot [%s] (id %d) but wasn't updated in UI"),
		// 	*Item->GetItemType().ToString(), *EquipmentSlot->GetEquipmentTypeTag().ToString(), static_cast<int>(EquipmentSlot->GetSlotId()));

		const float TileSize = GetTileSize();
		if (UInventoryEquippedSlottedItemWidget* EquippedSlottedItem = EquippedItemSlotWidget->OnItemEquipped(Item, EquipmentSlot->GetEquipmentTypeTag(), TileSize))
		{
			EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &UInventoryWidgetSpatial::EquippedSlottedItemClicked);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipDeSync: Item [%s] was unequipped but is still displayed as equipped in slot [%s] (id %d)"),
			*Item->GetItemType().ToString(), *EquippedItemSlotWidget->GetEquipmentTypeTag().ToString(), static_cast<int>(EquippedItemSlotWidget->GetSlotId()));
	}
}

void UInventoryWidgetSpatial::EquippedGridSlotClicked(UInventoryEquippedGridSlot* GridSlot, const FGameplayTag& EquipmentTypeTag)
{
	LOG_NETFUNCTIONCALL_MSG(TEXT("EquipSlot [%d]; HoverItem [%s]"), static_cast<int32>(GridSlot->GetSlotId()),
		GetHoverItem() ? *GetInventoryItemId(GetHoverItem()->GetInventoryItem()) : TEXT("None"));

	// Check to see if we can equip the Hover Item
	if (!CanEquipHoverItem(GridSlot, EquipmentTypeTag))
		return;

	// Create an Equipped Slotted Item and add it to the Equipped Grid Slot
	const float TileSize = GetTileSize();
	UInventoryItem* ItemToEquip = GetHoverItem()->GetInventoryItem();
	if (UInventoryEquippedSlottedItemWidget* EquippedSlottedItem = GridSlot->OnItemEquipped(ItemToEquip, EquipmentTypeTag, TileSize))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &UInventoryWidgetSpatial::EquippedSlottedItemClicked);

		InventoryGrid_Equipment->ClearHoverItem();

		// Inform the server that we've equipped an item (potentially unequipping an item as well)
		UInventoryComponent* InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer());
		check(IsValid(InventoryComponent));

		InventoryComponent->EquipItem(ItemToEquip, nullptr, GridSlot->GetSlotId());
	}
}

void UInventoryWidgetSpatial::EquippedSlottedItemClicked(UInventoryEquippedSlottedItemWidget* EquippedSlottedItem)
{
	// Remove the Item Description
	UInventoryStatics::ItemUnhovered(GetOwningPlayer());

	const auto HoverItem = GetHoverItem();
	if (IsValid(HoverItem) && HoverItem->IsStackable())
		return;

	UInventoryItem* ItemToEquip = IsValid(HoverItem) ? HoverItem->GetInventoryItem() : nullptr;
	UInventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();

	LOG_NETFUNCTIONCALL_MSG(TEXT("ItemToEquip [%s]; ItemToUnequip [%s]"), *GetInventoryItemId(ItemToEquip), *GetInventoryItemId(ItemToUnequip));

	// To avoid putting the hover item back to the inventory
	ActiveGrid->ClearHoverItem();

	// The equipment grid should be active to be able to put the item to the inventory, otherwise it will be lost
	ShowEquipmentGrid();

	// Get the Equipped Grid Slot holding this item
	UInventoryEquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	checkf(EquippedGridSlot != nullptr, TEXT("EquippedGridSlot must exist if there is EquippedSlottedItem"));
	
	// Clear the equipped grid slot of this item (set its inventory item to nullptr)
	ClearSlotOfItem(EquippedGridSlot);

	// Assign previously equipped item as the hover item
	InventoryGrid_Equipment->AssignHoverItem(ItemToUnequip);
	
	// Remove of the equipped slotted item from the equipped grid slot
	RemoveEquippedSlottedItem(EquippedSlottedItem);
	
	// Make a new equipped slotted item (for the item we held in HoverItem)
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);
	
	// Broadcast delegates for OnItemEquipped/OnItemUnequipped (from the IC)
	BroadcastClickedDelegates(ItemToEquip, ItemToUnequip, EquippedGridSlot->GetSlotId());
}

void UInventoryWidgetSpatial::DisableButton(UButton* Button)
{
	Button_Equipment->SetIsEnabled(true);
	Button_Consumable->SetIsEnabled(true);
	Button_Crafting->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UInventoryWidgetSpatial::SetActiveGrid(UInventoryGridWidget* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->OnHide();
	}
	DisableButton(Button);
	GridSwitcher->SetActiveWidget(Grid);
	ActiveGrid = Grid;
}

UInventoryGridWidget* UInventoryWidgetSpatial::GetInventoryGridByCategory(const FGameplayTag& ItemCategory) const
{
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Equipment))
		return InventoryGrid_Equipment;
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Consumable))
		return InventoryGrid_Consumable;
	if (ItemCategory.MatchesTagExact(InventoryTags::Inventory_ItemCategory_Crafting))
		return InventoryGrid_Crafting;
	return nullptr;
}

UInventoryItemDescription* UInventoryWidgetSpatial::GetOrCreateItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInventoryItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		//CanvasPanel->AddChildToCanvas(ItemDescription);
	}
	return ToRawPtr(ItemDescription);
}

UInventoryItemDescription* UInventoryWidgetSpatial::GetOrCreateEquippedItemDescription()
{
	if (!IsValid(EquippedItemDescription))
	{
		EquippedItemDescription = CreateWidget<UInventoryItemDescription>(GetOwningPlayer(), EquippedItemDescriptionClass);
		//CanvasPanel->AddChildToCanvas(EquippedItemDescription);
	}
	return ToRawPtr(EquippedItemDescription);
}

bool UInventoryWidgetSpatial::CanEquipHoverItem(const UInventoryEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid())
		return false;

	const UInventoryHoverItemWidget* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem))
		return false;

	const UInventoryItem* HeldItem = HoverItem->GetInventoryItem();
	return UInventoryStatics::CanEquipItem(HeldItem, EquipmentTypeTag);
}

UInventoryEquippedGridSlot* UInventoryWidgetSpatial::FindSlotWithEquippedItem(const UInventoryItem* EquippedItem) const
{
	auto* FoundEquippedSlot = EquippedGridSlots.FindByPredicate([EquippedItem](const UInventoryEquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem() == EquippedItem;
	});
	return FoundEquippedSlot ? FoundEquippedSlot->Get() : nullptr;
}

void UInventoryWidgetSpatial::ClearSlotOfItem(UInventoryEquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->ClearEquippedSlot();
	}
}

void UInventoryWidgetSpatial::RemoveEquippedSlottedItem(UInventoryEquippedSlottedItemWidget* EquippedSlottedItem)
{
	if (IsValid(EquippedSlottedItem))
	{
		if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this, &UInventoryWidgetSpatial::EquippedSlottedItemClicked))
		{
			EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this, &UInventoryWidgetSpatial::EquippedSlottedItemClicked);
		}
		EquippedSlottedItem->RemoveFromParent();
	}
}

void UInventoryWidgetSpatial::MakeEquippedSlottedItem(const UInventoryEquippedSlottedItemWidget* EquippedSlottedItem,
	UInventoryEquippedGridSlot* EquippedGridSlot, UInventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot) || !IsValid(ItemToEquip))
		return;

	if (UInventoryEquippedSlottedItemWidget* NewSlottedItem =
		EquippedGridSlot->OnItemEquipped(ItemToEquip, EquippedSlottedItem->GetEquipmentTypeTag(), GetTileSize()))
	{
		NewSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &UInventoryWidgetSpatial::EquippedSlottedItemClicked);
		EquippedGridSlot->SetEquippedSlottedItem(NewSlottedItem);
	}
}

void UInventoryWidgetSpatial::BroadcastClickedDelegates(UInventoryItem* ItemToEquip, UInventoryItem* ItemToUnequip, EInventoryEquipmentSlot SlotId) const
{
	if (const auto InventoryComponent = UInventoryStatics::GetInventoryComponent(GetOwningPlayer()))
	{
		InventoryComponent->EquipItem(ItemToEquip, ItemToUnequip, SlotId);
	}
}

// bool UInventoryWidgetSpatial::IsItemEquipped(const UInventoryItem* Item) const
// {
// 	return FindSlotWithEquippedItem(Item) != nullptr;
// }
//
// bool UInventoryWidgetSpatial::CanEquipItem(const UInventoryItem* Item) const
// {
// 	return FindItemBestEquipType(Item).IsValid();
// }

FGameplayTag UInventoryWidgetSpatial::FindItemBestEquipType(const UInventoryItem* Item) const
{
	if (IsValid(Item) && !Item->IsStackable() && Item->IsEquipable())
	{
		for (const auto& EquippedGridSlot : EquippedGridSlots)
		{
			if (Item->GetItemType().MatchesTag(EquippedGridSlot->GetEquipmentTypeTag()))
			{
				return EquippedGridSlot->GetEquipmentTypeTag();
			}
		}
	}
	return FGameplayTag::EmptyTag;
}

// UInventoryEquippedSlottedItemWidget* UInventoryWidgetSpatial::FindEquippedSlottedItemFor(const UInventoryItem* Item) const
// {
// 	if (const UInventoryEquippedGridSlot* EquippedSlot = FindSlotWithEquippedItem(Item))
// 	{
// 		return EquippedSlot->GetEquippedSlottedItem();
// 	}
// 	return nullptr;
// }
//
// UInventoryItem* UInventoryWidgetSpatial::GetItemInSlot(const FGameplayTag& EquipmentTypeTag) const
// {
// 	if (const auto* EquippedGridSlot = FindEquippedGridSlotByType(EquipmentTypeTag))
// 	{
// 		return EquippedGridSlot->GetInventoryItem().Get();
// 	}
// 	return nullptr;
// }

UInventoryEquippedGridSlot* UInventoryWidgetSpatial::FindEquippedGridSlotByType(const FGameplayTag& EquipmentTypeTag) const
{
	auto* EquippedGridSlotPtr = EquippedGridSlots.FindByPredicate([EquipmentTypeTag](const UInventoryEquippedGridSlot* GridSlot)
	{
		return GridSlot->GetEquipmentTypeTag().MatchesTagExact(EquipmentTypeTag);
	});
	return EquippedGridSlotPtr ? EquippedGridSlotPtr->Get() : nullptr;
}

UInventoryEquippedGridSlot* UInventoryWidgetSpatial::FindEquippedGridSlot(EInventoryEquipmentSlot SlotId) const
{
	auto* EquippedGridSlotPtr = EquippedGridSlots.FindByPredicate([SlotId](const UInventoryEquippedGridSlot* GridSlot)
	{
		return GridSlot->GetSlotId() == SlotId;
	});
	return EquippedGridSlotPtr ? EquippedGridSlotPtr->Get() : nullptr;
}

#if WITH_EDITOR
void UInventoryWidgetSpatial::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
	if (!ItemDescriptionClass)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(" has no ItemDescriptionClass specified.")));
	}
	if (!EquippedItemDescriptionClass)
	{
		CompileLog.Error(FText::FromString(GetName() + TEXT(" has no EquippedItemDescription specified.")));
	}
}
#endif//WITH_EDITOR
