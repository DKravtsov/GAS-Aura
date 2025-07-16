// Copyright 4sandwiches


#include "InventoryManagement/FastArray/InventoryEquipmentSlotFastArray.h"

#include "DebugHelper.h"

TArray<FInventoryEquipmentSlot> FInventoryEquipmentSlotFastArray::GetAllItems() const
{
	TArray<FInventoryEquipmentSlot> Result;
	Result.Reserve(Entries.Num());
	for (const auto& Entry : Entries)
	{
		Result.Add(Entry.Data);
	}
	return Result;
}

void FInventoryEquipmentSlotFastArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	LOG_NETFUNCTIONCALL_STRUCT_MSG(OwnerComponent, EquipmentSlots, TEXT(" (num = %d)"), AddedIndices.Num())
}

void FInventoryEquipmentSlotFastArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	LOG_NETFUNCTIONCALL_STRUCT_MSG(OwnerComponent, EquipmentSlots, TEXT(" (num = %d)"), ChangedIndices.Num())
}

const FInventoryEquipmentSlot* FInventoryEquipmentSlotFastArray::GetSlotById(EInventoryEquipmentSlot SlotId) const
{
	for (const auto& Entry : Entries)
	{
		if (Entry.Data.GetSlotId() == SlotId)
			return &Entry.Data;
	}
	return nullptr;
}

FInventoryEquipmentSlot* FInventoryEquipmentSlotFastArray::GetSlotByIdMutable(EInventoryEquipmentSlot SlotId)
{
	for (auto& Entry : Entries)
	{
		if (Entry.Data.GetSlotId() == SlotId)
		{
			MarkItemDirty(Entry);
			return &Entry.Data;
		}
	}
	return nullptr;
}

void FInventoryEquipmentSlotFastArray::AddSlot(EInventoryEquipmentSlot SlotId, const FGameplayTag& EquipmentTypeTag)
{
	auto& NewSlot = Entries.AddDefaulted_GetRef();
	NewSlot.Data.Init(SlotId, EquipmentTypeTag);
	MarkItemDirty(NewSlot);
}

const FInventoryEquipmentSlot* FInventoryEquipmentSlotFastArray::GetSlotByItem(const UInventoryItem* Item) const
{
	for (const auto& Entry : Entries)
	{
		if (Entry.Data.GetInventoryItem().Get() == Item)
			return &Entry.Data;
	}
	return nullptr;
}
