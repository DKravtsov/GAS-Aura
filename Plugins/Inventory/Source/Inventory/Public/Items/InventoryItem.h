// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class UInventoryItem : public UObject
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(BaseStruct="/Script/Inventory.InventoryItemManifest"), Replicated)
	FInstancedStruct ItemManifest;

	UPROPERTY(Replicated)
	int32 TotalStackCount = 0;

	mutable TOptional<bool> bCachedIsStackable = false;

public:

	void SetItemManifest(const FInventoryItemManifest& InItemManifest);

	const FInventoryItemManifest& GetItemManifest() const { return ItemManifest.Get<FInventoryItemManifest>(); }
	FInventoryItemManifest& GetItemManifestMutable() { return ItemManifest.GetMutable<FInventoryItemManifest>();}

	//~ Begin of UObject interface
	virtual bool IsSupportedForNetworking() const override {return true;}
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End of UObject interface

	template<typename TFragment>
	static const TFragment* GetFragment(UInventoryItem* Item, const FGameplayTag& FragmentTag)
	{
		if (Item != nullptr)
		{
			const FInventoryItemManifest& Manifest = Item->GetItemManifest();
			return Manifest.GetFragmentOfTypeWithTag<TFragment>(FragmentTag);
		}
		return nullptr;
	}

	bool IsStackable() const;

	const FGameplayTag& GetItemType() const;

	int32 GetTotalStackCount() const { return TotalStackCount; }
	void SetTotalStackCount(int32 InStackCount) { TotalStackCount = InStackCount; }
};
