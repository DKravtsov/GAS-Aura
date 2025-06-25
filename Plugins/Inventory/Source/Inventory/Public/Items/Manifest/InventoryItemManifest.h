// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "InventoryItemManifest.generated.h"

struct FInventoryItemFragment;
/**
 * Contains all the necessary data for creating a new Inventory item
 */
USTRUCT(BlueprintType)
struct FInventoryItemManifest
{
	GENERATED_BODY()

private:

	friend class UInventoryItemComponent;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Inventory.ItemCategory"))
	FGameplayTag ItemCategory;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems"))
	FGameplayTag ItemType;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<AActor> PickupActorClass;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FInventoryItemFragment>> Fragments;
	
public:
	INVENTORY_API FInventoryItemManifest();

	const FGameplayTag& GetItemCategory() const {return ItemCategory;}
	const FGameplayTag& GetItemType() const {return ItemType;}

	INVENTORY_API class UInventoryItem* Manifest(UObject* NewOuter);
	INVENTORY_API AActor* SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation) const;

	INVENTORY_API void AssimilateInventoryFragments(class UInventoryCompositeBase* Composite) const;

	template<class TFragment> requires std::derived_from<TFragment, FInventoryItemFragment>
	const TFragment* GetFragmentOfType() const
	{
		for (const auto& Fragment : Fragments)
		{
			if (const TFragment* FragmentPtr = Fragment.GetPtr<TFragment>())
			{
				return FragmentPtr;
			}
		}
		return nullptr;
	}

	template<class TFragment> requires std::derived_from<TFragment, FInventoryItemFragment>
	TFragment* GetFragmentOfTypeMutable()
	{
		for (auto& Fragment : Fragments)
		{
			if (TFragment* FragmentPtr = Fragment.GetMutablePtr<TFragment>())
			{
				return FragmentPtr;
			}
		}
		return nullptr;
	}

	template<class TFragment> requires std::derived_from<TFragment, FInventoryItemFragment>
	const TFragment* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
	{
		for (const auto& Fragment : Fragments)
		{
			if (const TFragment* FragmentPtr = Fragment.GetPtr<TFragment>())
			{
				if (FragmentPtr->GetFragmentTag().MatchesTag(FragmentTag))
				{
					return FragmentPtr;
				}
			}
		}
		return nullptr;
	}

	template<class TFragment> requires std::derived_from<TFragment, FInventoryItemFragment>
	TArray<const TFragment*> GetAllFragmentsOfType() const
	{
		TArray<const TFragment*> Result;
		Result.Reserve(Fragments.Num());
		for (const auto& Fragment : Fragments)
		{
			if (const TFragment* FragmentPtr = Fragment.GetPtr<TFragment>())
			{
				Result.Add(FragmentPtr);
			}
		}
		return Result;
	}
};
