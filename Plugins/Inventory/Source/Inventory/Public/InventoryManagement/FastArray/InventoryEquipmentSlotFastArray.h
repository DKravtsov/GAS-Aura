// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryEquipmentSlotFastArray.generated.h"

/// Single entry of equipment slot for Fast Array Serializer
USTRUCT(BlueprintType)
struct FInventoryEquipmentSlotEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:

	friend class UInventoryComponent; 
	friend struct FInventoryEquipmentSlotFastArray;

	UPROPERTY()
	FInventoryEquipmentSlot Data;

public:

	operator const FInventoryEquipmentSlot* () const {return &Data;}
};

/// List of inventory items
USTRUCT(BlueprintType)
struct FInventoryEquipmentSlotFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

private:
	friend class UInventoryComponent;
	
	// Replicated list of items
	UPROPERTY()
	TArray<FInventoryEquipmentSlotEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

public:

	FInventoryEquipmentSlotFastArray(UActorComponent* InOwnerComponent = nullptr) : OwnerComponent(InOwnerComponent) {}

	TArray<FInventoryEquipmentSlot> GetAllItems() const;

	//~ Begin of FFastArraySerializer interface (not virtual ones)
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer interface
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryEquipmentSlotEntry, FInventoryEquipmentSlotFastArray>( Entries, DeltaParms, *this );
	}

	template <typename Predicate>
	[[nodiscard]] FORCEINLINE const FInventoryEquipmentSlot* FindByPredicate(Predicate Pred) const
	{
		for (const FInventoryEquipmentSlotEntry* RESTRICT SlotData = Entries.GetData(), *RESTRICT DataEnd = SlotData + Entries.Num(); SlotData != DataEnd; ++SlotData)
		{
			if (::Invoke(Pred, SlotData->Data))
			{
				return &SlotData->Data;
			}
		}
		return nullptr;
	}

	const FInventoryEquipmentSlot* GetSlotByItem(const UInventoryItem* Item) const;
	const FInventoryEquipmentSlot* GetSlotById(EInventoryEquipmentSlot SlotId) const;
	// Returns equipment slot by id for edit, but also marks it dirty!
	FInventoryEquipmentSlot* GetSlotByIdMutable(EInventoryEquipmentSlot SlotId);
	
	void AddSlot(EInventoryEquipmentSlot SlotId, const FGameplayTag& EquipmentTypeTag);
};

template<>
struct TStructOpsTypeTraits<FInventoryEquipmentSlotFastArray > : public TStructOpsTypeTraitsBase2<FInventoryEquipmentSlotFastArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
