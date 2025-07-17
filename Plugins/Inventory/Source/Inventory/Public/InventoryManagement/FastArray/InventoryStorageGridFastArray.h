// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryStorageGridFastArray.generated.h"

/// Single entry of grid slot for Fast Array Serializer
USTRUCT(BlueprintType)
struct FInventoryStorageGridSlotEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:

	friend class UInventoryStorageGrid; 
	friend struct FInventoryStorageGridFastArray;

	UPROPERTY()
	FInventoryStorageGridSlot Data;

public:

};

USTRUCT(NotBlueprintType)
struct FInventoryStorageGridFastArray: public FFastArraySerializer
{
	GENERATED_BODY()

private:
	friend class UInventoryStorageGrid; 
	
	// Replicated list of items
	UPROPERTY()
	TArray<FInventoryStorageGridSlotEntry>	Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UInventoryStorageGrid> OwningGrid;

public:

	FInventoryStorageGridFastArray(UInventoryStorageGrid* Owner = nullptr) : OwningGrid(Owner) {}

	TArray<FInventoryStorageGridSlot> GetAllSlots() const;

	void AddSlot(int32 InTileIndex);
	void AddNumberSlots(int32 Count);

	const FInventoryStorageGridSlot& GetSlot(int32 InTileIndex) const {return Entries[InTileIndex].Data;}

	int32 GetStackCount(int32 SlotIndex) const;
	void AddStackCount(int32 SlotIndex, int32 AmountToAdd);
	void SetStackCount(int32 SlotIndex, int32 NewStackCount);

	void SetInventoryItem(int32 SlotIndex, UInventoryItem* NewItem, int32 NewStartIndex = INDEX_NONE);

	void ClearSlot(int32 SlotIndex);

	bool IsValidIndex(int32 Index) const {return Entries.IsValidIndex(Index);}

	bool IsSlotAvailable(int32 SlotIndex) const;

	//~ Begin of FFastArraySerializer interface (NOTE: intentionally not virtual; invoked via templated code)
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryStorageGridSlotEntry, FInventoryStorageGridFastArray>( Entries, DeltaParms, *this );
	}
	//~ End of FFastArraySerializer interface

};

template<>
struct TStructOpsTypeTraits< FInventoryStorageGridFastArray > : public TStructOpsTypeTraitsBase2< FInventoryStorageGridFastArray >
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};
