// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryFastArray.generated.h"

class UInventoryItem;

/// Single entry in inventory for Fast Array Serializer
USTRUCT(BlueprintType)
struct FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:

	friend class UInventoryComponent; 
	friend struct FInventoryFastArray;
	
	UPROPERTY()
	TObjectPtr<UInventoryItem> Item = nullptr;

public:

	FInventoryEntry();
};

/// List of inventory items
USTRUCT(BlueprintType)
struct FInventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

private:
	friend class UInventoryComponent;
	
	// Replicated list of items
	UPROPERTY()
	TArray<FInventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

public:

	FInventoryFastArray() : OwnerComponent(nullptr) {}
	explicit FInventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	TArray<UInventoryItem*> GetAllItems() const;

	//~ Begin of FFastArraySerializer interface (not virtual ones)
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer interface
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryEntry, FInventoryFastArray>( Entries, DeltaParms, *this );
	}

	UInventoryItem* AddItem(class UInventoryItemComponent* ItemComponent);
	UInventoryItem* AddItem(UInventoryItem* Item);
	void RemoveItem(UInventoryItem* Item);
};

template<>
struct TStructOpsTypeTraits< FInventoryFastArray > : public TStructOpsTypeTraitsBase2< FInventoryFastArray >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
